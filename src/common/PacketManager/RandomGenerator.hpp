#pragma once

#include <random>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

class RandomGenerator
{
private:
    std::random_device m_RandomDevice; // For generating random values
    std::mt19937 m_RandomNumGenerator; // For generating random numbers

public:
    RandomGenerator();
    ~RandomGenerator() = default;

    std::string generateRandomMac();
    std::string generateRandomIP(bool isIPv6 = false);
    uint16_t generateRandomPort();
    std::string generateRandomPayload(size_t size);
};