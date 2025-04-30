#pragma once

#include <rte_timer.h>
#include <rte_cycles.h>
#include <rte_lcore.h>

#include <functional>
#include <chrono>
#include <stdexcept>
#include <rte_config.h>

/// @brief A template class to create a timer for a fixed duration (in seconds).
template <int SECONDS>
class DpdkTimer
{
private:
    rte_timer m_Timer;
    std::function<void()> m_Callback;
    uint64_t m_TickInterval;
    uint64_t m_StartTime;

public:
    DpdkTimer() : m_StartTime(0)
    {

        rte_timer_init(&m_Timer);

        // Calculate the tick interval based on the requested SECONDS value and DPDK's timer frequency
        m_TickInterval = SECONDS * rte_get_timer_hz();
    }

    ~DpdkTimer() = default;

    /// @brief Returns the timer tick interval.
    inline uint64_t getTickInterval() const { return m_TickInterval; }

    /// @brief Returns true if timer is running, false otherwise.
    inline bool isTimerActive() const { return m_Timer.status.state == RTE_TIMER_PENDING; }

    /// @brief Returns true if timer is paused, false otherwise.
    inline bool isTimerPaused() const { return m_Timer.status.state == RTE_TIMER_STOP; }

    /// @brief Starts the timer.
    void startTimer(std::function<void()> callback, unsigned int coreId)
    {
        if (!callback)
        {
            throw std::invalid_argument("Error! Callback function is null");
        }

        // Store the callback function.
        m_Callback = std::move(callback);
        m_StartTime = rte_get_tsc_cycles();

        if (rte_timer_reset(&m_Timer, m_TickInterval, SINGLE, coreId, &DpdkTimer::_onTimeout, this) != 0)
        {
            throw std::runtime_error("Failed to start DPDK timer.");
        }
    }

    /// @brief Resets the timer.
    void resetTimer(unsigned int coreId)
    {
        startTimer(m_Callback, coreId);
    }

    /// @brief Stops the timer.
    void stopTimer(unsigned int coreId)
    {
        rte_timer_stop(&m_Timer);
    }

    /// @brief Manages the timer events and trigger the callback when needed.
    void manageTimer()
    {
        rte_timer_manage();
    }

    /// @brief Returns the elapsed time in cycles since the timer was started or resumed
    uint64_t getElapsedTime() const
    {
        if (!isTimerActive())
        {
            return 0;
        }
        return rte_get_tsc_cycles() - m_StartTime;
    }

private:
    /// @brief Callback function that is called when the timer ticks.
    static void
    _onTimeout(rte_timer *, void *arg)
    {
        static_cast<DpdkTimer *>(arg)->m_Callback();
    }
};
