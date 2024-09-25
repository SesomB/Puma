#pragma once
#include "cli/cli.h"
#include "cli/clifilesession.h"
#include <memory>
#include "spdlog/spdlog.h"

class CLIManager
{
private:
    std::unique_ptr<cli::Menu> m_RootMenu;
    std::unique_ptr<cli::Cli> m_Cli;

public:
    CLIManager();
    ~CLIManager() {};

    /// @brief Starts the CLI Session
    void Start();

private:
    void SetupCommands();
};