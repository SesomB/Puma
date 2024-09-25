#include "CLIManager.hpp"

CLIManager::CLIManager()
{
    // Initialize root menu
    m_RootMenu = std::make_unique<cli::Menu>("cli");

    // Setup commends
    SetupCommands();

    // Initialzie CLI
    m_Cli = std::make_unique<cli::Cli>(std::move(m_RootMenu));
}

void CLIManager::SetupCommands()
{
    m_RootMenu->Insert("Hello", [](std::ostream &out)
                       { out << "Hello World Test\n"; }, "Print Hello World");
    m_RootMenu->Insert("Exit", [](std::ostream &out)
                       { out << "Exiting...\n"; std::exit(0); }, "Exit Interactive Cli");
}

void CLIManager::Start()
{
    // Create a CLI sesssion and start it
    cli::CliFileSession inputCli(*m_Cli, std::cin, std::cout);
    inputCli.Start();
}
