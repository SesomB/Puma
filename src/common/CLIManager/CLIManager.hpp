#pragma once

#include "cli/cli.h"
#include "cli/clifilesession.h"
#include "spdlog/spdlog.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <stdexcept>

class CLIManager
{
private:
    std::unique_ptr<cli::Menu> m_RootMenu;
    std::unique_ptr<cli::Cli> m_CliApp;
    std::unordered_map<std::string, cli::Menu *> m_Menus;

public:
    /// @brief Constructor to initialize CLIManager with a root menu name and description
    CLIManager(const std::string &rootMenuName, const std::string &rootMenuDescription);

    /// @brief Destructor (defaulted, as resources are managed with unique_ptr)
    ~CLIManager() {};

    /// @brief Adds a new menu under the specified parent menu
    void addMenu(const std::string &parentMenuName, const std::string &newMenuName, const std::string &description);

    /// @brief Template function to add a command to the specified menu, accepting any callable type
    template <typename Callable>
    void addCommandToMenu(const std::string &menuName, const std::string &commandName, Callable &&commandFunction, const std::string &description)
    {
        // Find the menu by name
        cli::Menu *menu = findMenu(menuName);
        if (menu)
        {
            menu->Insert(commandName, [commandFunction](std::ostream &out)
                         { out << commandFunction << std::endl; }, description);
        }
        else
        {
            throw std::runtime_error(fmt::format("Menu {} not initialized.", menuName));
        }
    };

    cli::Menu *getMenuReference(const std::string &name)
    {
        return findMenu(name);
    };

    /// @brief Starts the CLI application
    void startCLI();

private:
    /// @brief Helper function to find a menu by its name
    cli::Menu *findMenu(const std::string &name);
};