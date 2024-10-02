#include "CLIManager.hpp"

CLIManager::CLIManager(const std::string &rootMenuName, const std::string &rootMenuDescription)
{
    // Initialize CLI root menu
    m_RootMenu = std::make_unique<cli::Menu>(rootMenuName, rootMenuDescription);
    m_Menus[rootMenuName] = m_RootMenu.get();
};

void CLIManager::addMenu(const std::string &parentMenuName, const std::string &newMenuName, const std::string &description)
{
    cli::Menu *parentMenu = findMenu(parentMenuName);
    if (parentMenu == nullptr)
    {
        // Default to root menu if not found
        parentMenu = m_RootMenu.get();
    }

    // Create and add the new sub menu
    auto newMenu = std::make_unique<cli::Menu>(newMenuName, description);
    m_Menus[newMenuName] = newMenu.get();
    parentMenu->Insert(std::move(newMenu));
};

void CLIManager::startCLI()
{
    // Create the CLI app
    m_CliApp = std::make_unique<cli::Cli>(std::move(m_RootMenu));
    if (!m_CliApp)
    {
        throw std::runtime_error("CLI App is not initialized.");
    }
    m_CliApp->EnterAction([](std::ostream &out)
                          { out << "Welcome to Puma Interactive CLI\n"; });
    m_CliApp->ExitAction([](std::ostream &out)
                         { out << "Exiting CLI...\n"; });

    // Create and start the CLI session
    cli::CliFileSession inputSession(*m_CliApp);
    inputSession.Start();
};

cli::Menu *CLIManager::findMenu(const std::string &name)
{
    auto it = m_Menus.find(name);
    return (it != m_Menus.end()) ? it->second : nullptr;
};