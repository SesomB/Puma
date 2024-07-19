#include "configuration.hpp"

Configuration::Configuration()
{
    m_ConfigurationBufferA = {
        .id = 0,
        .data = 10
    };

    m_ConfigurationBufferB = {
        .id = 1,
        .data = 20
    };

    m_ActiveConfiguration = &m_ConfigurationBufferA;
    m_InactiveConfiguration = &m_ConfigurationBufferB;
}

void Configuration::SwitchBuffer()
{
    if(m_ActiveConfiguration == &m_ConfigurationBufferA)
    {
        m_ActiveConfiguration = &m_ConfigurationBufferB;
        m_InactiveConfiguration = &m_ConfigurationBufferA;
        return;
    }

    if(m_ActiveConfiguration == &m_ConfigurationBufferB)
    {
        m_ActiveConfiguration = &m_ConfigurationBufferA;
        m_InactiveConfiguration = &m_ConfigurationBufferB;
        return;
    }

    throw std::logic_error("Reached invalid switch buffer code");

}