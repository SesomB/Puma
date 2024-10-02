#include "PacketManager.hpp"

void PacketManager::createPackets(size_t numOfPackets,
                                  const std::optional<std::vector<std::string>> &srcMacs,
                                  const std::optional<std::vector<std::string>> &dstMacs,
                                  const std::optional<std::vector<std::string>> &srcIps,
                                  const std::optional<std::vector<std::string>> &dstIps,
                                  const std::optional<std::vector<TransportProtocol>> &protocols,
                                  const std::optional<std::vector<uint16_t>> &srcPorts,
                                  const std::optional<std::vector<uint16_t>> &dstPorts,
                                  const std::optional<std::vector<std::string>> &payloads)
{
    // Clear existing packets
    m_Packets.clear();

    for (size_t i = 0; i < numOfPackets; i++)
    {
        // Reset the custom packet for a new packet in each iteration
        m_CustomPacket.reset();

        // Add Layers
        _addEthernetLayer(i, srcMacs, dstMacs);
        _addIPLayer(i, srcIps, dstIps);
        _addTransportLayer(i, protocols, srcPorts, dstPorts);
        _addPayloadLayer(i, payloads);

        // Create and aggregate packets
        m_CustomPacket.buildPacket();
        m_Packets.emplace_back(m_CustomPacket.getPacket());
    }
};

void PacketManager::_addEthernetLayer(size_t index, const std::optional<std::vector<std::string>> &srcMacs,
                                      const std::optional<std::vector<std::string>> &dstMacs)
{
    std::optional<std::string> srcMAC;
    std::optional<std::string> dstMAC;

    // Handle Source MAC
    if (srcMacs.has_value() && index < srcMacs->size())
    {
        srcMAC = srcMacs->at(index);
    }
    else
    {
        srcMAC = std::nullopt;
    }

    // Handle Destination MAC
    if (dstMacs.has_value() && index < dstMacs->size())
    {
        dstMAC = dstMacs->at(index);
    }
    else
    {
        dstMAC = std::nullopt;
    }

    // Add layer to packet
    m_CustomPacket.addEthernetLayer(srcMAC, dstMAC);
};

void PacketManager::_addIPLayer(size_t index, const std::optional<std::vector<std::string>> &srcIps,
                                const std::optional<std::vector<std::string>> &dstIps)
{
    std::optional<std::string> srcIP;
    std::optional<std::string> dstIP;

    // Handle Source IP
    if (srcIps.has_value() && index < srcIps->size())
    {
        srcIP = srcIps->at(index);
    }
    else
    {
        srcIP = std::nullopt;
    }

    // Handle Destination IP
    if (dstIps.has_value() && index < dstIps->size())
    {
        dstIP = dstIps->at(index);
    }
    else
    {
        dstIP = std::nullopt;
    }

    // Add layer to packet
    m_CustomPacket.addIPLayer(srcIP, dstIP);
};

void PacketManager::_addTransportLayer(size_t index, const std::optional<std::vector<TransportProtocol>> &protocols,
                                       const std::optional<std::vector<uint16_t>> &srcPorts,
                                       const std::optional<std::vector<uint16_t>> &dstPorts)
{
    TransportProtocol protocol;
    std::optional<uint16_t> srcPort;
    std::optional<uint16_t> dstPort;

    // Handle Transport Protocol
    if (protocols.has_value() && index < protocols->size())
    {
        protocol = protocols->at(index);
    }
    else
    {
        protocol = TransportProtocol::TCP;
    }

    // Handle Source Ports
    if (srcPorts.has_value() && index < srcPorts->size())
    {
        srcPort = srcPorts->at(index);
    }
    else
    {
        srcPort = std::nullopt;
    }

    // Handle Destination Ports
    if (dstPorts.has_value() && index < dstPorts->size())
    {
        dstPort = dstPorts->at(index);
    }
    else
    {
        dstPort = std::nullopt;
    }

    // Add layer to packet
    m_CustomPacket.addTransportLayer(protocol, srcPort, dstPort);
};

void PacketManager::_addPayloadLayer(size_t index, const std::optional<std::vector<std::string>> &payloads)
{
    std::optional<std::string> payload;
    std::optional<size_t> size;

    // Handle Payload
    if (payloads.has_value() && index < payloads->size())
    {
        payload = payloads->at(index);
        size = payload->size();
    }
    else
    {
        payload = std::nullopt;
        size = std::nullopt;
    }

    // Add layer to packet
    m_CustomPacket.addPayloadLayer(payload, size);
};