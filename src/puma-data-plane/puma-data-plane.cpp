#include "puma-data-plane.hpp"

PumaDataPlane::PumaDataPlane(Configuration &configuration, pcpp::DpdkDevice *device, int rxQueueId)
    : m_Configuration(configuration), m_Device(device), m_RxQueueId(rxQueueId), m_KeepRunning(false),
      m_CoreId(MAX_NUM_OF_CORES + 1)
{
}

bool PumaDataPlane::run(uint32_t coreId)
{
    // Register coreId for this worker
    m_CoreId = coreId;
    m_KeepRunning = true;

    std::cout << "Puma Data Plane Launched on core #" << m_CoreId << std::endl;
    // initialize a mbuf packet array of size 64
    pcpp::MBufRawPacket *mbufs[64] = {};

    // endless loop, until asking the thread to stop
    while (m_KeepRunning)
    {
        // receive packets from RX device
        uint16_t numOfPackets = m_Device->receivePackets(mbufs, 64, 0);

        if (numOfPackets == 0)
            continue;

        for (int i = 0; i < numOfPackets; i++)
        {
            pcpp::Packet packet(mbufs[i], false);
            // packet.setRawPacket(mbufs[i], false);

            rte_pktmbuf_free(mbufs[i]->getMBuf());
        }
    }

    return true;
}

void PumaDataPlane::stop()
{
    m_KeepRunning = false;
}

uint32_t PumaDataPlane::getCoreId() const
{
    return m_CoreId;
}
