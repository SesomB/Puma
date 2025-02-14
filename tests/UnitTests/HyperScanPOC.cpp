#include "spdlog/spdlog.h"
#include "gtest/gtest.h"
#include <hs/hs.h>

#include <bitset>
#include <cstring>
#include <hs/hs.h>
#include <hs/hs_common.h>
#include <hs/hs_runtime.h>
#include <new>
#include <rte_common.h>
#include <rte_malloc.h>
#include <string>
#include <vector>

#define MAX_RULES 256

using PatternId = std::pair<uint16_t, uint16_t>;

class RegexPattern
{
  public:
    std::string mRegexPattern;
    uint32_t mScanFlags;
    PatternId mId;
};

class UserRule
{
  public:
    std::vector<std::string> mRegexPatterns;

    UserRule(const std::vector<std::string> &regex_patterns) : mRegexPatterns(regex_patterns){};
    ~UserRule() = default;
};

class HyperCompiler
{
  private:
    static constexpr uint32_t BIAS_COMPILE_FLAGS = HS_FLAG_DOTALL;

    hs_database_t *m_Database;
    std::vector<RegexPattern> m_RegexPatterns;

  public:
    HyperCompiler()
        : m_Database(nullptr){
              // Set Custom Allocators
              // hs_set_allocator(DpdkHyperScannerAllocator::dpdk_alloc, DpdkHyperScannerAllocator::dpdk_free);
          };

    ~HyperCompiler()
    {
        hs_free_database(m_Database);
    }

    const auto &getDatabase() const
    {
        return m_Database;
    }

    void addPattern(const RegexPattern &&regex_pattern)
    {
        m_RegexPatterns.emplace_back(regex_pattern);
    }

    bool compile_database()
    {
        uint32_t regex_patterns_count = m_RegexPatterns.size();

        std::vector<const char *> patterns(regex_patterns_count);
        std::vector<uint32_t> flags(regex_patterns_count);
        std::vector<uint32_t> ids(regex_patterns_count);

        for (int i = 0; i < regex_patterns_count; ++i)
        {
            auto &regex_pattern = m_RegexPatterns[i];

            patterns[i] = regex_pattern.mRegexPattern.c_str();
            flags[i] = regex_pattern.mScanFlags | BIAS_COMPILE_FLAGS;
            ids[i] = *reinterpret_cast<uint32_t *>(&regex_pattern.mId);
        }

        hs_compile_error_t *compileErr;

        if (hs_compile_multi(patterns.data(), flags.data(), ids.data(), regex_patterns_count, HS_MODE_BLOCK, nullptr,
                             &m_Database, &compileErr) != HS_SUCCESS)
        {
            spdlog::error("Hyperscan compile error: {}", compileErr->message);
            return false;
        }

        return true;
    }
};

class HyperScanner
{
  private:
    using uint64_t = unsigned long long;

    const hs_database *const &m_DatabaseRef;
    hs_scratch_t *m_Scratch;
    std::bitset<MAX_RULES> m_ResultBitset;

  public:
    HyperScanner(const hs_database_t *const &database_reference) : m_DatabaseRef(database_reference)
    {
        auto error_code = hs_alloc_scratch(m_DatabaseRef, &m_Scratch);

        if (error_code != HS_SUCCESS)
        {
            spdlog::error("Could not allocate scrath pad: {}", error_code);
            throw std::bad_alloc();
        }
    }

    ~HyperScanner()
    {
        hs_free_scratch(m_Scratch);
    }

    const auto &getResults() const
    {
        return m_ResultBitset;
    }

    bool scan(const uint8_t *payload, uint32_t payload_size)
    {
        static constexpr uint32_t scan_flags = 0;

        if (!m_DatabaseRef)
            return false;

        if (hs_scan(m_DatabaseRef, reinterpret_cast<const char *>(payload), payload_size, scan_flags, m_Scratch,
                    _matchHandler, reinterpret_cast<void *>(this)) != HS_SUCCESS)
        {
            spdlog::error("Scan Error.");
            return false;
        }

        return true;
    };

  private:
    static int _matchHandler(uint32_t id, uint64_t from, uint64_t to, uint32_t flags, void *hyperscanner_ptr)
    {
        HyperScanner *current_object = reinterpret_cast<HyperScanner *>(hyperscanner_ptr);
        const auto &[rule_id, pattern_id_in_rule] = *reinterpret_cast<PatternId *>(&id);

        current_object->m_ResultBitset.set(rule_id);

        return 0;
    };
};

class HyperScanTest : public ::testing::Test
{
  protected:
    std::vector<UserRule> m_UserRules = {
        UserRule({"hello", "this"}),
        UserRule({"world", "test"}),
        UserRule({"example", "string"}),
        UserRule({"pattern", "search"}),
        UserRule({"12345", "67890"}),
        UserRule({R"(\d{3,5})"}),
        UserRule({"complex_pattern_here", "another_complex"}),
    };

    HyperCompiler m_Compiler;

    void SetUp() override
    {
        // Add patterns for different rule IDs
        for (uint16_t rule_id = 0; rule_id < m_UserRules.size(); rule_id++)
        {
            for (uint16_t pattern_index_in_rule = 0; pattern_index_in_rule < m_UserRules[rule_id].mRegexPatterns.size();
                 pattern_index_in_rule++)
            {
                auto &user_rule_regex_pattern = m_UserRules[rule_id].mRegexPatterns[pattern_index_in_rule];

                m_Compiler.addPattern(RegexPattern{user_rule_regex_pattern, 0, {rule_id, pattern_index_in_rule}});
            }
        }

        ASSERT_TRUE(m_Compiler.compile_database());
    }
};

TEST_F(HyperScanTest, MatchSinglePattern)
{

    std::string input = "hello world!";

    const uint8_t *input_ptr = reinterpret_cast<const uint8_t *>(input.c_str());
    const uint32_t input_size = input.size();

    HyperScanner hyperscanner(m_Compiler.getDatabase());

    ASSERT_TRUE(hyperscanner.scan(input_ptr, input_size));

    const auto &results = hyperscanner.getResults();

    EXPECT_TRUE(results.test(0));
    EXPECT_TRUE(results.test(1));
    EXPECT_EQ(results.count(), 2);
};

TEST_F(HyperScanTest, MatchDifferentRules)
{
    std::string input = "hello world example pattern 12345";

    const uint8_t *input_ptr = reinterpret_cast<const uint8_t *>(input.c_str());
    const uint32_t input_size = input.size();

    HyperScanner hyperscanner(m_Compiler.getDatabase());

    ASSERT_TRUE(hyperscanner.scan(input_ptr, input_size));

    const auto &results = hyperscanner.getResults();

    EXPECT_TRUE(results.test(0)); // "hello"
    EXPECT_TRUE(results.test(1)); // "world"
    EXPECT_TRUE(results.test(2)); // "example"
    EXPECT_TRUE(results.test(3)); // "pattern"
    EXPECT_TRUE(results.test(4)); // "12345"

    EXPECT_EQ(results.count(), 6); // "12345"
};

TEST_F(HyperScanTest, MatchNoPatterns)
{
    std::string input = "nothing matched here";

    const uint8_t *input_ptr = reinterpret_cast<const uint8_t *>(input.c_str());
    const uint32_t input_size = input.size();

    HyperScanner hyperscanner(m_Compiler.getDatabase());

    ASSERT_TRUE(hyperscanner.scan(input_ptr, input_size));

    const auto &results = hyperscanner.getResults();

    EXPECT_EQ(results.count(), 0);
};
