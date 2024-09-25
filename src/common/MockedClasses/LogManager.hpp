#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <iostream>
#include <vector>
#include <string>

const std::string LOG_FILE = "log_manager.txt";

class LogManager
{

private:
    std::shared_ptr<spdlog::logger> m_Logger;
    std::vector<std::string> m_AggregatedLogs;
    bool m_LogToFile;

public:
    LogManager(bool logToFile = false);

    void PrintAllLogs();

    /// @brief  Helper method to log messages and aggregate them into a vector
    void Log(const std::string msg, spdlog::level::level_enum log_level);

    /// @brief Run method to create mock logs
    void Run();

    /// @brief Retrieve all aggregated logs.
    const std::vector<std::string> getAllLogs() const { return m_AggregatedLogs; }
};