#pragma once

#include <PcapFileDevice.h>
#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <TcpLayer.h>
#include <UdpLayer.h>
#include <PayloadLayer.h>
#include <Packet.h>
#include <vector>
#include <string>
#include <optional>
#include <arpa/inet.h>
#include "spdlog/spdlog.h"
#include "RandomGenerator.hpp"

enum class TransportProtocol
{
    TCP,
    UDP
};

/// @brief This class is responsible for creating all the needed Packet layers, allowing manual and random data input.
class CustomPacket
{
private:
    pcpp::Packet m_Packet;
    RandomGenerator m_RandomGen;

public:
    CustomPacket() {};
    ~CustomPacket() = default;

    void reset() { m_Packet = pcpp::Packet(); };

    /// @brief Adds an Ethernet layer with source and destination MAC Addresses
    void addEthernetLayer(const std::optional<std::string> &srcMac = std::nullopt, const std::optional<std::string> &dstMac = std::nullopt);

    /// @brief Adds an IP layer with source and destination IP Addresses
    void addIPLayer(const std::optional<std::string> &srcIp = std::nullopt,
                    const std::optional<std::string> &dstIp = std::nullopt);

    /// @brief Adds a Transport layer with Protocol, source and destination Ports
    void addTransportLayer(std::optional<TransportProtocol> = TransportProtocol::TCP, const std::optional<uint16_t> &srcPort = std::nullopt, const std::optional<uint16_t> &dstPort = std::nullopt);

    /// @brief Adds a Payload layer with payload data
    void addPayloadLayer(const std::optional<std::string> &payload = std::nullopt, std::optional<size_t> size = 50);

    void buildPacket();

    const pcpp::Packet &getPacket() const { return m_Packet; };

private:
    std::string _payloadToString(const std::vector<uint8_t> &payload);
};