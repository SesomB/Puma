#include <exception>
#include <cstdlib>
#include "gtest/gtest.h"
#include "CLIManager/CLIManager.hpp"
#include "MockedClasses/LogManager.hpp"
#include "MockedClasses/SystemMetrics.hpp"

const std::string ROOT_MENU_NAME = "PumaMain";
const std::string ROOT_MENU_DESCRIPTION = "Puma Interactive CLI Main Menu.";

// Test method to run system scripts
std::string runScript(const std::string &scriptPath)
{
    std::string command = "bash \"" + scriptPath + "\"";

    // Open a pipe to the command
    std::array<char, 128> buffer;
    std::string result;

    // Use popen to execute the command
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    // Read the output from the script
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();    // Append to the result string
        std::cout << buffer.data(); // Print each line to stdout
    }

    return result; // Return the full output as a string
}

TEST(InteractiveCli, example)
{

    // Create LogManger class with some mocked logs
    LogManager logManager(false);
    logManager.setLogLevel(spdlog::level::debug);
    logManager.generateMockedLogs();

    // Create SystemMetrics class
    SystemMetrics systemMetrics;
    logManager.Log(systemMetrics.getRAMUsage(), spdlog::level::debug);
    logManager.Log(systemMetrics.getCPUUsage(), spdlog::level::debug);
    logManager.Log(systemMetrics.getMemoryUsage(), spdlog::level::debug);
    logManager.Log(systemMetrics.getSystemUptime(), spdlog::level::debug);

    // Create Puma Interactive CLI
    CLIManager cliManager(ROOT_MENU_NAME, ROOT_MENU_DESCRIPTION);

    // Add Menu and commands for SystemMetrics class
    cliManager.addMenu(ROOT_MENU_NAME, "metrics", "System Metrics related commands.");
    cliManager.addCommandToMenu("metrics", "ram_usage", systemMetrics.getRAMUsage(), "Returns the system current RAM Usage.");
    cliManager.addCommandToMenu("metrics", "cpu_usage", systemMetrics.getCPUUsage(), "Returns the system current CPU Usage.");
    cliManager.addCommandToMenu("metrics", "memory_usage", systemMetrics.getMemoryUsage(), "Returns the system current Memory Usage.");
    cliManager.addCommandToMenu("metrics", "uptime", systemMetrics.getSystemUptime(), "Returns the system Up Time.");

    // Add Menu and commands for LogManger class
    cliManager.addMenu(ROOT_MENU_NAME, "logs", "LogManager related commands");
    cliManager.addCommandToMenu("logs", "print_logs", logManager.printAllLogs(), "Returns the system current RAM Usage.");

    // Add nested menu
    cliManager.addMenu("metrics", "NestedMenu", "Test Add Menu Heirarchy.");

    // Add menu for control scripts
    std::string initDpdk = "/home/dev/Dev/Puma/scripts/dpdk_init.sh";
    std::string configureHugepages = "/home/dev/Dev/Puma/scripts/configure_hugepages.sh";
    cliManager.addMenu(ROOT_MENU_NAME, "scripts", "System Control Scripts.");
    auto scriptMenu = cliManager.getMenuReference("scripts");
    scriptMenu->Insert("init", [this, initDpdk](std::ostream &out)
                       { runScript(initDpdk); }, "Initializes DPDK.");
    scriptMenu->Insert("huge_pages", [this, configureHugepages](std::ostream &out)
                       { runScript(configureHugepages); }, "Configures DPDK Memory Huge Pages.");

    // Run CLI App
    cliManager.startCLI();
}
