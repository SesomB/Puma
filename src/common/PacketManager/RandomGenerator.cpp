#include "RandomGenerator.hpp"

RandomGenerator::RandomGenerator() : m_RandomNumGenerator(m_RandomDevice()) {};

pcpp::MacAddress RandomGenerator::generateRandomMac()
{
    std::uniform_int_distribution<> dist(0, 255);
    std::stringstream ss;
    for (int i = 0; i < 6; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << dist(m_RandomNumGenerator);
        if (i != 5)
            ss << ":";
    }
    return pcpp::MacAddress(ss.str());
};

std::string RandomGenerator::generateRandomIP(bool isIPv6)
{
    std::stringstream ss;
    // For IPv6
    if (isIPv6)
    {
        std::uniform_int_distribution<uint16_t> dist(0, 65535);
        for (int i = 0; i < 8; i++)
        {
            ss << std::hex << dist(m_RandomNumGenerator);
            if (i != 7)
                ss << ":";
        }
        return ss.str();
    }
    // For IPv4
    else
    {
        std::uniform_int_distribution<> dist(1, 254);
        ss << dist(m_RandomNumGenerator) << '.' << dist(m_RandomNumGenerator) << '.' << dist(m_RandomNumGenerator) << '.' << dist(m_RandomNumGenerator);
        return ss.str();
    }
};

uint16_t RandomGenerator::generateRandomPort()
{
    std::uniform_int_distribution<uint16_t> dist(1, 65535);
    return dist(m_RandomNumGenerator);
};

std::string RandomGenerator::generateRandomPayload(size_t size)
{
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<size_t> dist(0, characters.size() - 1);
    std::string payload;
    for (size_t i = 0; i < size; i++)
    {
        payload += characters[dist(m_RandomNumGenerator)];
    }
    return payload;
};