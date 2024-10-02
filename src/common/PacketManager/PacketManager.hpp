#pragma once

#include <vector>
#include <optional>
#include <PcapFileDevice.h>
#include "CustomPacket.hpp"

class PacketManager
{
private:
    CustomPacket m_CustomPacket;
    std::vector<pcpp::Packet> m_Packets;

public:
    PacketManager() {};
    ~PacketManager() = default;

    void createPackets(size_t numOfPackets = 10,
                       const std::optional<std::vector<std::string>> &srcMacs = std::nullopt,
                       const std::optional<std::vector<std::string>> &dstMacs = std::nullopt,
                       const std::optional<std::vector<std::string>> &srcIps = std::nullopt,
                       const std::optional<std::vector<std::string>> &dstIps = std::nullopt,
                       const std::optional<std::vector<TransportProtocol>> &protocols = std::nullopt,
                       const std::optional<std::vector<uint16_t>> &srcPorts = std::nullopt,
                       const std::optional<std::vector<uint16_t>> &dstPorts = std::nullopt,
                       const std::optional<std::vector<std::string>> &payloads = std::nullopt);

    const std::vector<pcpp::Packet> &getPackets() const { return m_Packets; };

private:
    void _addEthernetLayer(size_t index, const std::optional<std::vector<std::string>> &srcMacs,
                           const std::optional<std::vector<std::string>> &dstMacs);
    void _addIPLayer(size_t index, const std::optional<std::vector<std::string>> &srcIps,
                     const std::optional<std::vector<std::string>> &dstIps);
    void _addTransportLayer(size_t index, const std::optional<std::vector<TransportProtocol>> &protocols,
                            const std::optional<std::vector<uint16_t>> &srcPorts,
                            const std::optional<std::vector<uint16_t>> &dstPorts);
    void _addPayloadLayer(size_t index, const std::optional<std::vector<std::string>> &payloads);
};