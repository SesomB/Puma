#include "SystemMetrics.hpp"

SystemMetrics::SystemMetrics(bool logToFile) : m_SystemInfo{}
{
    sysinfo(&m_SystemInfo);
};

std::string SystemMetrics::getRAMUsage() const
{
    long totalRam = m_SystemInfo.totalram / (1024 * 1024); // Convert to MB
    long freeRam = m_SystemInfo.freeram / (1024 * 1024);   // Convert to MB
    long usedRam = totalRam - freeRam;

    std::ostringstream oss;
    oss << "Total RAM: " << totalRam << " MB, Used RAM: " << usedRam << " MB, Free RAM: " << freeRam << " MB";
    return oss.str();
};

std::string SystemMetrics::getCPUUsage() const
{
    auto lines = readFileLines("/proc/stat");
    std::istringstream ss(lines[0]);
    std::string cpuLabel;
    long user, nice, system, idle;

    ss >> cpuLabel >> user >> nice >> system >> idle;
    long total = user + nice + system + idle;

    std::ostringstream oss;
    oss << "CPU: user=" << user << " nice=" << nice << " system=" << system << " idle=" << idle << " total=" << total;
    return oss.str();
};

std::string SystemMetrics::getMemoryUsage() const
{
    long totalMemory = m_SystemInfo.totalram / (1024 * 1024); // MB
    long freeMemory = m_SystemInfo.freeram / (1024 * 1024);   // MB
    long usedMemory = totalMemory - freeMemory;

    std::ostringstream oss;
    oss << "Total Memory: " << totalMemory << " MB, Used Memory: " << usedMemory << " MB, Free Memory: " << freeMemory << " MB";
    return oss.str();
};

std::string SystemMetrics::getSystemUptime() const
{
    long uptime = m_SystemInfo.uptime / 60; // Convert to minutes
    long days = uptime / (24 * 60);
    long hours = (uptime % (24 * 60)) / 60;
    long minutes = uptime % 60;

    std::ostringstream oss;
    oss << "System Uptime: " << days << " days, " << hours << " hours, " << minutes << " minutes";
    return oss.str();
};

std::vector<std::string> SystemMetrics::readFileLines(const std::string &filepath) const
{
    std::ifstream file(filepath);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    return lines;
};

std::string SystemMetrics::readFile(const std::string &filepath) const
{
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
};