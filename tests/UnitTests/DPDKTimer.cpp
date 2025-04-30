#include "spdlog/spdlog.h"
#include "gtest/gtest.h"
#include <atomic>
#include <thread>
#include <rte_eal.h>
#include <rte_timer.h>
#include <rte_jhash.h>

#include "DPDKTimer/DPDKTimer.hpp"

class DPDKTimerTest : public ::testing::Test
{

protected:
    DpdkTimer<3> mTimer;
    std::atomic<int> firedCount = 0;
    std::atomic<bool> isFired = false;
    using Clock = std::chrono::steady_clock;
    Clock::time_point startTime;
    Clock::time_point endTime;

    void callback()
    {
        endTime = Clock::now();
        firedCount++;
        isFired = true;
        spdlog::info("Callback executed at end time");
    };

    void SetUp() override
    {
        static bool isInitialized = false;
        if (!isInitialized)
        {
            int argc = 1;
            const char *argv[] = {"test"};
            ASSERT_GE(rte_eal_init(argc, const_cast<char **>(argv)), 0) << "Failed to initialize DPDK EAL";
            ASSERT_EQ(rte_timer_subsystem_init(), 0) << "Failed to initialize DPDK timer subsystem";
            isInitialized = true;
        }
    }
};

TEST_F(DPDKTimerTest, TimerResetAndFireAfter3Seconds)
{
    isFired = false;
    mTimer.startTimer([this]
                      { callback(); }, rte_lcore_id());

    EXPECT_TRUE(mTimer.isTimerActive());
    EXPECT_FALSE(mTimer.isTimerPaused());

    // Reset the timer 3 times
    for (int i = 0; i < 3; ++i)
    {
        mTimer.resetTimer(rte_lcore_id());
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Sleep between resets
    }
    // Allow the timer to fire
    mTimer.manageTimer();
    std::this_thread::sleep_for(std::chrono::seconds(3 + 1)); // Let the timer fire

    // Verify that the timer fired exactly once
    EXPECT_EQ(firedCount.load(), 1);
};

TEST_F(DPDKTimerTest, TimerFiresApproximatelyAfter3Seconds)
{
    isFired = false;
    spdlog::info("Timer TickInterval: {}", mTimer.getTickInterval());
    startTime = Clock::now();

    // Start the timer with the callback
    mTimer.startTimer([this]
                      { callback(); }, rte_lcore_id());

    EXPECT_TRUE(mTimer.isTimerActive());

    // Wait for the timer to fire
    while (!isFired.load())
    {
        mTimer.manageTimer();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Measure the elapsed time and check if the callback executed in approximately 3 seconds
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    spdlog::info("Timer test completed. Elapsed time: {} ms", elapsed);

    // We expect the elapsed time to be close to 3000 milliseconds (3 seconds)
    EXPECT_GE(elapsed, 2900);
    EXPECT_LE(elapsed, 3100);
    EXPECT_EQ(firedCount.load(), 1);
};

TEST_F(DPDKTimerTest, TcpSessionTimeout)
{
    spdlog::info("Starting TCP session timeout simulation");

    // Simulated 5-tuple as a string
    const std::string fiveTuple = "10.0.0.1:1234-10.0.0.2:80-TCP";
    uint32_t hash = rte_jhash(fiveTuple.data(), fiveTuple.size(), 0);

    mTimer.startTimer([this]
                      { callback(); }, rte_lcore_id());

    EXPECT_TRUE(mTimer.isTimerActive());

    // Simulate packet arrivals resetting the timer
    for (int i = 0; i < 3; ++i)
    {
        mTimer.resetTimer(rte_lcore_id());
        spdlog::info("Simulated packet {} from session hash {}", i + 1, hash);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        mTimer.manageTimer();
    }

    // Wait long enough to trigger timeout
    for (int i = 0; i < 40; ++i)
    {
        mTimer.manageTimer();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_TRUE(isFired);
    EXPECT_FALSE(mTimer.isTimerActive());
};