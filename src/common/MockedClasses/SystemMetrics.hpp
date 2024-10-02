#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h> // For sysconf
#include <iomanip>  // For formatting
#include <sys/sysinfo.h>

class SystemMetrics
{
private:
    struct sysinfo m_SystemInfo;

public:
    SystemMetrics(bool logToFile = false);
    ~SystemMetrics() {};

    std::string getRAMUsage() const;
    std::string getCPUUsage() const;
    std::string getMemoryUsage() const;
    std::string getSystemUptime() const;

private:
    std::vector<std::string> readFileLines(const std::string &filePath) const;
    std::string readFile(const std::string &filePath) const;
};