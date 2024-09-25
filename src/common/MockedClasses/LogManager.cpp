#include "LogManager.hpp"

LogManager::LogManager(bool logToFile) : m_LogToFile(logToFile)
{
    // Create a console sink
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S] - [%^%l%$] --- %v");

    // Create a file sink
    if (m_LogToFile)
    {
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_FILE, true);
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S] - [%l] --- %v");

        // Combine console and file sinks into one logger
        m_Logger = std::make_shared<spdlog::logger>("multi_sink_logger", spdlog::sinks_init_list{consoleSink, fileSink});
    }
    else
    {
        m_Logger = std::make_shared<spdlog::logger>("console_logger", spdlog::sinks_init_list{consoleSink});
    }

    spdlog::set_default_logger(m_Logger);
    std::string msg = "LogManager initialized.";
    m_Logger->info(msg);
    m_AggregatedLogs.push_back(msg);
};

void LogManager::printAllLogs()
{
    std::cout << "\nLogs:\n";
    int index = 1;
    for (const auto &log : m_AggregatedLogs)
    {
        std::cout << index << ". " << log << "\n";
        index++;
    }
};

void LogManager::Log(const std::string &msg, spdlog::level::level_enum log_level)
{
    switch (log_level)
    {
    case spdlog::level::info:
        m_Logger->info(msg);
        break;
    case spdlog::level::debug:
        m_Logger->debug(msg);
        break;
    case spdlog::level::warn:
        m_Logger->warn(msg);
        break;
    case spdlog::level::err:
        m_Logger->error(msg);
        break;

    default:
        break;
    }

    m_AggregatedLogs.push_back(msg);
};

void LogManager::generateMockedLogs()
{
    // Generate some mocked logs for testing purposes
    Log("Starting Log Manager.", spdlog::level::info);
    Log("System initialized.", spdlog::level::debug);
    Log("Network connection lost.", spdlog::level::warn);
    Log("System failure.", spdlog::level::err);

    if (m_LogToFile)
    {
        std::cout << "Logs have been written to " << LOG_FILE << "\n";
    }
};