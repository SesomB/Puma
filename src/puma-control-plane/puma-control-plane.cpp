#include "puma-control-plane.hpp"

PumaControlPlane::PumaControlPlane()
{
    // Initialize DPDK
    pcpp::CoreMask coreMaskToUse = pcpp::createCoreMaskFromCoreIds(std::vector<int>{0, 1});
    pcpp::DpdkDeviceList::initDpdk(coreMaskToUse, MBUF_POOL_SIZE);
    // Find DPDK devices
    pcpp::DpdkDevice *device1 = pcpp::DpdkDeviceList::getInstance().getDeviceByPort(0);
    if (device1 == NULL)
    {
        spdlog::error("Cannot find device1 with port {}", 0);
        std::exit(1);
    }
    // Open DPDK devices
    if (!device1->openMultiQueues(1, 1))
    {
        spdlog::error("Cannot open device1 with port {}", 0);
        std::exit(1);
    }

    // Create worker threads
    int openedrxQueues = device1->getNumOfOpenedRxQueues();
    spdlog::info("Opened RX Queue count: {}", openedrxQueues);
    for (int rxQueue = 0; rxQueue < openedrxQueues; rxQueue++)
    {
        m_PumaDataPlanes.emplace_back(new PumaDataPlane(m_Configuration, device1, rxQueue));
    }

    // Create core mask - use core 1 and 2 for the two threads
    for (int i = 1; i <= m_PumaDataPlanes.size(); i++)
    {
        m_PumaDataPlanesCoreMask = m_PumaDataPlanesCoreMask | (1 << i);
    }
}

void PumaControlPlane::Run()
{
    // Start capture in async mode
    if (!pcpp::DpdkDeviceList::getInstance().startDpdkWorkerThreads(m_PumaDataPlanesCoreMask, m_PumaDataPlanes))
    {
        spdlog::error("Couldn't start worker threads");
        std::exit(1);
    }

    // Main event loop
    while (m_KeepRunning)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    pcpp::DpdkDeviceList::getInstance().stopDpdkWorkerThreads();
}

void PumaControlPlane::Stop()
{
    m_KeepRunning = false;
}
