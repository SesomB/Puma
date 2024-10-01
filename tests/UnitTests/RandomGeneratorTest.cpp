#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "PacketManager/RandomGenerator.hpp"

TEST(RandomGeneratorTest, example)
{

    // Test Random MAC
    spdlog::info("Random Source MAC: {}", RandomGenerator().generateRandomMac().toString());
    spdlog::info("Random Destination MAC: {}", RandomGenerator().generateRandomMac().toString());

    // Test Random IPs
    spdlog::info("Random Source IPv4: {}", RandomGenerator().generateRandomIP(false));
    spdlog::info("Random Destination IPv4: {}", RandomGenerator().generateRandomIP(false));
    spdlog::info("Random Source IPv6: {}", RandomGenerator().generateRandomIP(true));
    spdlog::info("Random Destination IPv6: {}", RandomGenerator().generateRandomIP(true));

    // Test Random Port
    spdlog::info("Random Source Port: {}", RandomGenerator().generateRandomPort());
    spdlog::info("Random Destination Port: {}", RandomGenerator().generateRandomPort());

    // Test Random Payload
    std::string payload = RandomGenerator().generateRandomPayload(50);
    spdlog::info("Random Payload {} | Length {}", payload, payload.size());
    std::string payload1 = RandomGenerator().generateRandomPayload(100);
    spdlog::info("Random Payload {} | Length {}", payload1, payload1.size());
    std::string payload2 = RandomGenerator().generateRandomPayload(400);
    spdlog::info("Random Payload {} | Length {}", payload2, payload2.size());
}