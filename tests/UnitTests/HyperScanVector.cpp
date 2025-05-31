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
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Hyperscan
{
  private:
    static constexpr uint32_t BIAS_COMPILE_FLAGS = HS_FLAG_DOTALL;
    using uint64_t = unsigned long long;

  public:
    using Database = hs_database_t;
    using Scratchpad = hs_scratch_t;

    struct Pattern
    {
        std::string pattern;
        uint32_t flag;
        uint32_t id;
    };

    using Context = std::pair<const std::vector<std::string> &, const std::vector<Hyperscan::Pattern> &>;

    static bool compileDatabase(Database *&db, const std::vector<Pattern> &patterns, uint32_t mode = HS_MODE_BLOCK)
    {
        uint32_t patterns_count = patterns.size();

        std::vector<const char *> _patterns(patterns_count);
        std::vector<uint32_t> _flags(patterns_count);
        std::vector<uint32_t> _ids(patterns_count);

        for (int i = 0; i < patterns_count; i++)
        {
            const auto &pattern = patterns[i];
            _patterns[i] = pattern.pattern.c_str();
            _flags[i] = pattern.flag | BIAS_COMPILE_FLAGS;
            _ids[i] = pattern.id;
        }

        hs_compile_error_t *compileErr = nullptr;

        if (hs_compile_multi(_patterns.data(), _flags.data(), _ids.data(), patterns_count, mode, nullptr, &(db),
                             &compileErr) != HS_SUCCESS)
        {
            return false;
        }

        return true;
    };

    static bool allocateScratchpad(Scratchpad *&scrachpad, const Database *db)
    {
        return (hs_alloc_scratch(db, &scrachpad) == HS_SUCCESS);
    };

    // static bool scan(const uint8_t *data, const uint32_t length, const Database *database, Scratchpad *scratchpad,
    //                  void *context)
    // {
    //     uint32_t scan_flags = 0;
    //     auto scan_ret =
    //         hs_scan(database, reinterpret_cast<const char *>(data), length, scan_flags, scratchpad, NULL, context);
    //     if (scan_ret == HS_SUCCESS)
    //         return true;
    // };

    static int onScanHit(uint32_t id, uint64_t from, uint64_t to, uint32_t flags, void *context)
    {

        auto &[payloads, patterns] = *reinterpret_cast<Hyperscan::Context *>(context);

        int i = 0;
        uint64_t total_bytes = 0;
        while (total_bytes < to)
        {
            total_bytes += payloads[i++].size();
        }
        i--;

        spdlog::info("[HIT] dataIndex={} | id={} | from={} | to={} | flags={} | pattern={} | data={}", i, id, from, to,
                     flags, patterns[id].pattern, payloads[i]);
        return 0;
    };
};

class HyperScanVector : public ::testing::Test
{
  protected:
    std::vector<Hyperscan::Pattern> PATTERNS = {
        Hyperscan::Pattern{.pattern = "CCCCCCCCCC", .flag = 1, .id = 0},
        Hyperscan::Pattern{.pattern = "DDDDDDDDDD", .flag = 1, .id = 1},
        Hyperscan::Pattern{.pattern = "^GET", .flag = 1, .id = 2},
        Hyperscan::Pattern{.pattern = "23", .flag = 1, .id = 3},
        Hyperscan::Pattern{.pattern = "01234", .flag = 1, .id = 4},
    };

    Hyperscan::Database *m_Database = nullptr;
    Hyperscan::Scratchpad *m_Scratchpad = nullptr;
    void SetUp() override
    {
    }
};

// TEST_F(HyperScanVector, BlockScan)
// {

//     bool compileResult = Hyperscan::compileDatabase(m_Database, PATTERNS, HS_MODE_BLOCK);
//     ASSERT_TRUE(compileResult);

//     bool scratch_alloc_ret = (hs_alloc_scratch(m_Database, &m_Scratchpad) == HS_SUCCESS);
//     ASSERT_TRUE(scratch_alloc_ret);

//     std::string input = "0123456789";

//     const char *data = input.c_str();
//     const uint32_t data_length = input.size();

//     auto scan_ret = hs_scan(m_Database, data, data_length, 0, m_Scratchpad, Hyperscan::onScanHit, &PATTERNS);

//     ASSERT_EQ(scan_ret, HS_SUCCESS);
// };

TEST_F(HyperScanVector, VectorScan)
{

    bool compileResult = Hyperscan::compileDatabase(m_Database, PATTERNS, HS_MODE_VECTORED);
    ASSERT_TRUE(compileResult);

    bool scratch_alloc_ret = (hs_alloc_scratch(m_Database, &m_Scratchpad) == HS_SUCCESS);
    ASSERT_TRUE(scratch_alloc_ret);

    std::vector<std::string> inputs = {
        "", "", "", "AGET asdasdasasd", "", "CCCCCCCCCC", "BBBBBBBBBB", "CCCCCCCCCC", "DDDDDDDDDD", "EEEEEEEEEE",
    };

    // Buffer for data
    std::vector<const char *> data;
    data.reserve(inputs.size());

    // Buffer for lengths
    std::vector<uint32_t> data_lengths;
    data_lengths.reserve(inputs.size());

    // Populate
    for (const auto &input : inputs)
    {
        data.push_back(input.c_str());
        data_lengths.push_back(input.size());
    }

    Hyperscan::Context context = {inputs, PATTERNS};

    // Vector scan
    auto scan_ret = hs_scan_vector(m_Database, data.data(), data_lengths.data(), data.size(), 0, m_Scratchpad,
                                   Hyperscan::onScanHit, &context);

    ASSERT_EQ(scan_ret, HS_SUCCESS);
};
