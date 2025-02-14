#include <hs/hs.h>
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"

#include <hs/hs.h>
#include <vector>
#include <string>
#include <bitset>
#include <cstring>

#include <iostream>
#include <unordered_map>

#define MAX_RULES 256

struct sProcessedRule
{
    uint16_t ruleId;
    uint16_t ruleIndex;
};

struct sScanResult
{
    bool scanStatus = false;
    std::bitset<MAX_RULES> matchedRules;
    std::array<int, MAX_RULES> matchCounts; // For the number of matched patterns per rule
    /**
     * Tracks unique (ruleId, ruleIndex) pairs to avoid counting duplicate
     * matches.
     * Hyperscan calls the match callback for every occurrence of a pattern, * even if it's the same word appearing multiple times in different
     * locations.
     * Without this set, matchCounts[ruleId] would increment for each
     * occurrence rather than per unique pattern within the rule.
     */
    std::set<std::pair<uint16_t, uint16_t>> uniqueMatches;
    std::vector<sProcessedRule> matchedData;

    sScanResult() { memset(matchCounts.data(), 0, sizeof(matchCounts)); }
};

class HyperscanWrapper
{

private:
    std::vector<std::string> m_PatternList;
    std::vector<sProcessedRule> m_Rules;
    hs_database_t *m_Database;
    hs_scratch_t *m_Scratch;

public:
    HyperscanWrapper() : m_Database(nullptr), m_Scratch(nullptr) {}

    ~HyperscanWrapper()
    {
        if (m_Database)
            hs_free_database(m_Database);
        if (m_Scratch)
            hs_free_scratch(m_Scratch);
    }

    void addPattern(uint16_t ruleId, const std::vector<std::string> &patterns)
    {
        for (size_t i = 0; i < patterns.size(); ++i)
        {
            m_Rules.push_back({ruleId, static_cast<uint16_t>(i)});
            m_PatternList.push_back(patterns[i]);
        }
    }

    bool compile()
    {
        hs_compile_error_t *compileErr;
        std::vector<const char *> cPatterns;
        std::vector<unsigned int> cFlags(m_PatternList.size(), HS_FLAG_DOTALL);
        std::vector<unsigned int> ids;

        for (size_t i = 0; i < m_PatternList.size(); ++i)
        {
            cPatterns.push_back(m_PatternList[i].c_str());
            ids.push_back(*reinterpret_cast<const unsigned int *>(&m_Rules[i]));
        }

        if (hs_compile_multi(cPatterns.data(), cFlags.data(), ids.data(), m_PatternList.size(),
                             HS_MODE_BLOCK, nullptr, &m_Database, &compileErr) != HS_SUCCESS)
        {
            spdlog::error("Hyperscan compile error: {}", compileErr->message);

            return false;
        }

        if (hs_alloc_scratch(m_Database, &m_Scratch) != HS_SUCCESS)
        {
            spdlog::info("Failed to allocate Hyperscan scratch space");
            return false;
        }

        return true;
    }

    sScanResult scan(const std::string &text)
    {
        sScanResult result;
        if (!m_Database || !m_Scratch)
        {
            spdlog::error("Scan Error.");
        }

        if (hs_scan(m_Database, text.c_str(), text.size(), 0, m_Scratch, _matchHandler, &result) != HS_SUCCESS)
        {
            spdlog::error("Scan Error.");
        }

        return result;
    }

private:
    static int _matchHandler(unsigned int id, unsigned long long from, unsigned long long to, unsigned int, void *ctx)
    {
        auto *result = static_cast<sScanResult *>(ctx);
        const sProcessedRule rule = *reinterpret_cast<sProcessedRule *>(&id);
        spdlog::info("Match found: Rule ID {} | Pattern Index {}", rule.ruleId, rule.ruleIndex);

        // Ensure uniqueness in the matchedRules and matchCounts
        if (result->uniqueMatches.insert({rule.ruleId, rule.ruleIndex}).second)
        {
            result->matchedRules.set(rule.ruleId);
            result->matchCounts[rule.ruleId]++;
        }
        if (result->uniqueMatches.find({rule.ruleId, rule.ruleIndex}) != result->uniqueMatches.end())
        {
            result->matchedData.push_back(rule);
        }
        result->scanStatus = true;

        return 0;
    };
};

class HyperScanTest : public ::testing::Test
{
protected:
    HyperscanWrapper hs;

    void SetUp() override
    {
        // Add patterns for different rule IDs
        hs.addPattern(1, {"hello", "this"});
        hs.addPattern(2, {"world", "test"});
        hs.addPattern(3, {"example", "string"});
        hs.addPattern(4, {"pattern", "search"});
        hs.addPattern(5, {"12345", "67890"});
        hs.addPattern(6, {R"(\d{3,5})"});
        hs.addPattern(7, {"complex_pattern_here", "another_complex"});
        ASSERT_TRUE(hs.compile());
    }
};

TEST_F(HyperScanTest, MatchSinglePattern)
{

    std::string input = "hello world!";
    auto result = hs.scan(input);

    ASSERT_TRUE(result.scanStatus);
    EXPECT_TRUE(result.matchedRules.test(1));
    EXPECT_TRUE(result.matchedRules.test(2));
    EXPECT_EQ(result.matchCounts[1], 1);
    EXPECT_EQ(result.matchCounts[2], 1);
};

TEST_F(HyperScanTest, MatchDifferentRules)
{
    std::string input = "hello world example pattern 12345";
    auto result = hs.scan(input);

    EXPECT_TRUE(result.scanStatus);
    EXPECT_TRUE(result.matchedRules.test(1)); // "hello"
    EXPECT_TRUE(result.matchedRules.test(2)); // "world"
    EXPECT_TRUE(result.matchedRules.test(3)); // "example"
    EXPECT_TRUE(result.matchedRules.test(4)); // "pattern"
    EXPECT_TRUE(result.matchedRules.test(5)); // "12345"

    EXPECT_EQ(result.matchCounts[1], 1); // "hello"
    EXPECT_EQ(result.matchCounts[2], 1); // "world"
    EXPECT_EQ(result.matchCounts[3], 1); // "example"
    EXPECT_EQ(result.matchCounts[4], 1); // "pattern"
    EXPECT_EQ(result.matchCounts[5], 1); // "12345"

    // EXPECT_EQ(result.matchedData.size(), 5); // 5 unique pattern matches
};

TEST_F(HyperScanTest, MatchNoPatterns)
{
    std::string input = "nothing matched here";
    auto result = hs.scan(input);

    EXPECT_FALSE(result.matchedRules.test(1)); // Rule 1 should NOT match
    EXPECT_FALSE(result.matchedRules.test(2)); // Rule 2 should NOT match
    EXPECT_FALSE(result.matchedRules.test(3)); // Rule 3 should NOT match
    EXPECT_FALSE(result.matchedRules.test(4)); // Rule 4 should NOT match
    EXPECT_FALSE(result.matchedRules.test(5)); // Rule 5 should NOT match
    EXPECT_FALSE(result.matchedRules.test(6)); // Rule 6 should NOT match
    EXPECT_FALSE(result.matchedRules.test(7)); // Rule 7 should NOT match

    // Verify match counts
    EXPECT_EQ(result.matchCounts[1], 0); // Rule 1 should match 0 times
    EXPECT_EQ(result.matchCounts[2], 0); // Rule 2 should match 0 times
    EXPECT_EQ(result.matchCounts[3], 0); // Rule 3 should match 0 times
    EXPECT_EQ(result.matchCounts[4], 0); // Rule 4 should match 0 times
    EXPECT_EQ(result.matchCounts[5], 0); // Rule 5 should match 0 times

    EXPECT_EQ(result.matchedData.size(), 0); // No matches
};