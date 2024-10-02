#include "CustomPacket.hpp"

void CustomPacket::addEthernetLayer(const std::optional<std::string> &srcMac, const std::optional<std::string> &dstMac)
{
    // Handle source MAC
    pcpp::MacAddress sourceMac;
    if (srcMac.has_value())
    {
        sourceMac = pcpp::MacAddress(srcMac.value());
    }
    else
    {
        sourceMac = pcpp::MacAddress(m_RandomGen.generateRandomMac());
    }

    // Handle Destination MAC
    pcpp::MacAddress destinationMac;
    if (dstMac.has_value())
    {
        destinationMac = pcpp::MacAddress(dstMac.value());
    }
    else
    {
        destinationMac = pcpp::MacAddress(m_RandomGen.generateRandomMac());
    }

    // Create and add layer
    pcpp::EthLayer *ethLayer = new pcpp::EthLayer(sourceMac, destinationMac);
    m_Packet.addLayer(ethLayer);
};

void CustomPacket::addIPLayer(const std::optional<std::string> &srcIp,
                              const std::optional<std::string> &dstIp)
{
    pcpp::IPAddress sourceIp;
    pcpp::IPAddress destinationIp;

    // Both source and destination IPs have values
    if (srcIp.has_value() && dstIp.has_value())
    {
        sourceIp = pcpp::IPAddress(srcIp.value());
        destinationIp = pcpp::IPAddress(dstIp.value());

        // Validate
        if (!sourceIp.isIPv4() && !sourceIp.isIPv6())
        {
            spdlog::error("Invalid source IP address {}", srcIp.value());
            return;
        }

        if (!destinationIp.isIPv4() && !destinationIp.isIPv6())
        {
            spdlog::error("Invalid destination IP address {}", dstIp.value());
            return;
        }

        if (!(sourceIp.isIPv4() && destinationIp.isIPv4()) &&
            !(sourceIp.isIPv6() && destinationIp.isIPv6()))
        {
            spdlog::error("Source {} and Destination {} IPs must be of the same type", srcIp.value(), dstIp.value());
            return;
        }
    }

    // Only Source IP has a value
    else if (srcIp.has_value())
    {
        sourceIp = pcpp::IPAddress(srcIp.value());
        destinationIp = pcpp::IPAddress(m_RandomGen.generateRandomIP(!sourceIp.isIPv4()));
    }

    // Only Destination IP has a value
    else if (dstIp.has_value())
    {
        destinationIp = pcpp::IPAddress(dstIp.value());
        sourceIp = pcpp::IPAddress(m_RandomGen.generateRandomIP(!destinationIp.isIPv4()));
    }

    // Neither has a value, generate random IPv4
    else
    {
        sourceIp = pcpp::IPAddress(m_RandomGen.generateRandomIP(false));
        destinationIp = pcpp::IPAddress(m_RandomGen.generateRandomIP(false));
    }

    // Create and add layer
    // IPv6 case
    if (sourceIp.isIPv6() && destinationIp.isIPv6())
    {
        pcpp::IPv6Layer *ipv6Layer = new pcpp::IPv6Layer(sourceIp.toString(), destinationIp.toString());
        ipv6Layer->getIPv6Header()->hopLimit = 64; // Default hop limit
        m_Packet.addLayer(ipv6Layer);
    }
    // IPv4 case
    else
    {
        pcpp::IPv4Layer *ipv4Layer = new pcpp::IPv4Layer(sourceIp.toString(), destinationIp.toString());
        ipv4Layer->getIPv4Header()->timeToLive = 64; // Default TTL value
        m_Packet.addLayer(ipv4Layer);
    }
};

void CustomPacket::addTransportLayer(std::optional<TransportProtocol> protocol, const std::optional<uint16_t> &srcPort, const std::optional<uint16_t> &dstPort)
{
    // Handle Source Port
    uint16_t sourcePort;
    if (srcPort.has_value())
    {
        sourcePort = srcPort.value();
    }
    else
    {
        sourcePort = m_RandomGen.generateRandomPort();
    }

    // Handle Destination Port
    uint16_t destinationPort;
    if (dstPort.has_value())
    {
        destinationPort = dstPort.value();
    }
    else
    {
        destinationPort = m_RandomGen.generateRandomPort();
    }

    // Create and add layer
    // TCP case
    if (protocol == TransportProtocol::TCP)
    {
        pcpp::TcpLayer *tcpLayer = new pcpp::TcpLayer(sourcePort, destinationPort);
        tcpLayer->getTcpHeader()->synFlag = 1;
        tcpLayer->getTcpHeader()->windowSize = htons(8192);
        m_Packet.addLayer(tcpLayer);
    }
    // UDP case
    else if (protocol == TransportProtocol::UDP)
    {
        pcpp::UdpLayer *udpLayer = new pcpp::UdpLayer(sourcePort, destinationPort);
        m_Packet.addLayer(udpLayer);
    }
};

void CustomPacket::addPayloadLayer(const std::optional<std::string> &payload, std::optional<size_t> size)
{
    // Handle payload vector
    std::vector<uint8_t> payloadData;
    if (payload.has_value())
    {
        // Convert string to a vector of bytes
        payloadData.assign(payload->begin(), payload->end());
    }
    else
    {
        std::string randomPayload = m_RandomGen.generateRandomPayload();
        payloadData.assign(randomPayload.begin(), randomPayload.end());
    }

    // Create and add layer
    pcpp::PayloadLayer *payloadLayer = new pcpp::PayloadLayer(payloadData.data(), payloadData.size());
    m_Packet.addLayer(payloadLayer);
};

void CustomPacket::buildPacket()
{
    m_Packet.computeCalculateFields();
};
