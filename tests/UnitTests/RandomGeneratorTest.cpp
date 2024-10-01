#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "CustomPacket/RandomGenerator.hpp"

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
    std::vector<uint8_t> payload = RandomGenerator().generateRandomPayload(50);
    std::string payloadStr = RandomGenerator().payloadToString(payload);
    spdlog::info("Random Payload {} | Length {}", payloadStr, payloadStr.size());
    std::vector<uint8_t> payload1 = RandomGenerator().generateRandomPayload(100);
    std::string payload1Str = RandomGenerator().payloadToString(payload1);
    spdlog::info("Random Payload {} | Length {}", payload1Str, payload1Str.size());
}