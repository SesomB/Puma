#include "spdlog/spdlog.h"
#include "gtest/gtest.h"
#include <vector>

#include "SessionHandler/SessionHandler.hpp"

class Playground : public ::testing::Test
{
protected:
    std::unordered_map<uint32_t, SessionContext> mSessionsMap;

    std::vector<SessionTuple> session1{
        {"192.168.1.1", "10.0.0.1", 1234, 80, "TCP"},
        {"10.0.0.1", "192.168.1.1", 80, 1234, "TCP"},
        {"192.168.1.1", "10.0.0.1", 1234, 80, "TCP"},
        {"10.0.0.1", "192.168.1.1", 80, 1234, "TCP"},
        {"192.168.1.1", "10.0.0.1", 1234, 80, "TCP"}};

    std::vector<SessionTuple> session2{
        {"10.10.10.1", "10.10.10.2", 2222, 443, "TCP"},
        {"10.10.10.2", "10.10.10.1", 443, 2222, "TCP"},
        {"10.10.10.1", "10.10.10.2", 2222, 443, "TCP"},
        {"10.10.10.2", "10.10.10.1", 443, 2222, "TCP"},
        {"10.10.10.1", "10.10.10.2", 2222, 443, "TCP"}};

    std::vector<SessionTuple> session3{
        {"172.20.0.5", "172.20.0.10", 8080, 22, "TCP"},
        {"172.20.0.10", "172.20.0.5", 22, 8080, "TCP"},
        {"172.20.0.5", "172.20.0.10", 8080, 22, "TCP"},
        {"172.20.0.10", "172.20.0.5", 22, 8080, "TCP"},
        {"172.20.0.5", "172.20.0.10", 8080, 22, "TCP"}};

    // Add sessions here

    std::vector<std::vector<SessionTuple>> mAllSessions{session1, session2, session3};

    void SetUp() override
    {
    }
};

TEST_F(Playground, Playground)
{
    // int sessionIndex = 1;
    for (const auto &session : mAllSessions)
    {
        for (const auto &packet : session)
        {
            uint32_t key = makeSessionKey(packet);
            uint32_t reversedKey = makeSessionKey(packet.reversed());
            auto it = mSessionsMap.find(key);

            // Lookup miss
            if (it == mSessionsMap.end())
            {
                // Try reversed direction
                it = mSessionsMap.find(reversedKey);

                // Reversed Lookup miss
                if (it == mSessionsMap.end())
                {
                    mSessionsMap.emplace(key, SessionContext(packet, session.size()));
                }
                // Reversed Lookup hit
                else
                {
                    auto &context = it->second;
                    context.mSessionPacketCount++;

                    // reorder on insertion
                    context.mSessionMap[it->second.mExpectedSeq++] = packet;
                    // Timer reset
                }
            }

            // Lookup hit
            else
            {
                auto &context = it->second;
                context.mSessionPacketCount++;

                // reorder on insertion
                context.mSessionMap[it->second.mExpectedSeq++] = packet;

                // Timer reset
            }
        }
    }

    // Mocks timer::manage()
    for (auto &[key, state] : mSessionsMap)
    {
        state.callback();
    }

    // Validate
    for (const auto &[key, state] : mSessionsMap)
    {
        EXPECT_TRUE(state.mIsFinished); // will be true
        EXPECT_EQ(state.mSessionMap.size(), state.mSessionPacketCount);
    }

    // Address to mbuf_free

    ASSERT_EQ(mSessionsMap.size(), mAllSessions.size());
};