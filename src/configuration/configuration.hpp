#pragma once

#include <stdexcept>
#include <iostream>

struct configuration
{
    int id;
    int data;
};

class Configuration
{
    private:
    configuration m_ConfigurationBufferA;
    configuration m_ConfigurationBufferB;

    configuration* m_ActiveConfiguration; // Inline getter
    configuration* m_InactiveConfiguration; // Inline getter

    public:
    Configuration();

    ~Configuration(){};

    void SwitchBuffer();

    inline configuration& GetInactive() const { return *m_InactiveConfiguration; };
    inline const configuration& GetActive() const { return *m_ActiveConfiguration; };
};
