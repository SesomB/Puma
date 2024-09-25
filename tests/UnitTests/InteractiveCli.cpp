#include "gtest/gtest.h"
#include "CLIManager/CLIManager.hpp"
#include "MockedClasses/LogManager.hpp"

TEST(InteractiveCli, example)
{
    // Create some mocked logs
    LogManager logManager(false);
    logManager.Log("Test scope added log.", spdlog::level::info);
    logManager.Run();

    // logManager.PrintAllLogs();

    // CLIManager cliManager;
    // cliManager.Start();
}
