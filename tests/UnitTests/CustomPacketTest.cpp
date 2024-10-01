#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "PacketManager/CustomPacket.hpp"

class CustomPacketTest : public ::testing::Test
{
protected:
    CustomPacket customPacket;

    // Define data for reuse
    const std::string srcMac = "00:11:22:33:44:55";
    const std::string dstMac = "66:77:88:99:AA:BB";
    const std::string srcIpv4 = "192.168.1.1";
    const std::string dstIpv4 = "192.168.1.20";
    const std::string srcIpv6 = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    const std::string dstIpv6 = "2001:0db8:85a3:0000:0000:8a2e:0370:7335";
    const uint16_t srcPort = 443;
    const uint16_t dstPort = 80;
    std::string payload = "Hello World!";
};

// For Ethernet Layer with both MACs
TEST_F(CustomPacketTest, AddEthernetLayer_WithBothMacs)
{
    customPacket.addEthernetLayer(srcMac, dstMac);
    customPacket.buildPacket();

    const pcpp::Packet &packet = customPacket.getPacket();
    ASSERT_TRUE(packet.isPacketOfType(pcpp::Ethernet));
    ASSERT_EQ(packet.getLayerOfType<pcpp::EthLayer>()->getSourceMac(), pcpp::MacAddress(srcMac));
    ASSERT_EQ(packet.getLayerOfType<pcpp::EthLayer>()->getDestMac(), pcpp::MacAddress(dstMac));
}

// For Ethernet Layer with only source MAC
TEST_F(CustomPacketTest, AddEthernetLayer_WithOnlySrcMac)
{
    customPacket.addEthernetLayer(srcMac);
    customPacket.buildPacket();

    const pcpp::Packet &packet = customPacket.getPacket();
    ASSERT_TRUE(packet.isPacketOfType(pcpp::Ethernet));
    ASSERT_EQ(packet.getLayerOfType<pcpp::EthLayer>()->getSourceMac(), pcpp::MacAddress(srcMac));
}

// For Ethernet Layer with only destination MAC
TEST_F(CustomPacketTest, AddEthernetLayer_WithOnlyDstMac)
{
    customPacket.addEthernetLayer(std::nullopt, dstMac);
    customPacket.buildPacket();

    const pcpp::Packet &packet = customPacket.getPacket();
    ASSERT_TRUE(packet.isPacketOfType(pcpp::Ethernet));
    ASSERT_EQ(packet.getLayerOfType<pcpp::EthLayer>()->getDestMac(), pcpp::MacAddress(dstMac));
}

// For Ethernet Layer with no MACs
TEST_F(CustomPacketTest, AddEthernetLayer_WithNoMacs)
{
    customPacket.addEthernetLayer(std::nullopt, std::nullopt);
    customPacket.buildPacket();

    const pcpp::Packet &packet = customPacket.getPacket();
    ASSERT_TRUE(packet.isPacketOfType(pcpp::Ethernet));
}

// For IP Layer with both IPs
TEST_F(CustomPacketTest, AddIPLayer_WithBothIPs)
{
    // IPv4
    customPacket.addIPLayer(srcIpv4, dstIpv4);
    customPacket.buildPacket();

    const pcpp::Packet &ipv4Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv4Packet.isPacketOfType(pcpp::IPv4));
    ASSERT_EQ(ipv4Packet.getLayerOfType<pcpp::IPv4Layer>()->getSrcIPv4Address(), pcpp::IPv4Address(srcIpv4));
    ASSERT_EQ(ipv4Packet.getLayerOfType<pcpp::IPv4Layer>()->getDstIPv4Address(), pcpp::IPv4Address(dstIpv4));

    // IPv6
    customPacket.addIPLayer(srcIpv6, dstIpv6);
    customPacket.buildPacket();

    const pcpp::Packet &ipv6Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv6Packet.isPacketOfType(pcpp::IPv6));
    ASSERT_EQ(ipv6Packet.getLayerOfType<pcpp::IPv6Layer>()->getSrcIPv6Address(), pcpp::IPv6Address(srcIpv6));
    ASSERT_EQ(ipv6Packet.getLayerOfType<pcpp::IPv6Layer>()->getDstIPv6Address(), pcpp::IPv6Address(dstIpv6));
}

// For IP Layer with only source IPs
TEST_F(CustomPacketTest, AddIPLayer_WithOnlySrcIPs)
{
    // IPv4
    customPacket.addIPLayer(srcIpv4);
    customPacket.buildPacket();

    const pcpp::Packet &ipv4Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv4Packet.isPacketOfType(pcpp::IPv4));
    ASSERT_EQ(ipv4Packet.getLayerOfType<pcpp::IPv4Layer>()->getSrcIPv4Address(), pcpp::IPv4Address(srcIpv4));

    // IPv6
    customPacket.addIPLayer(srcIpv6);
    customPacket.buildPacket();

    const pcpp::Packet &ipv6Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv6Packet.isPacketOfType(pcpp::IPv6));
    ASSERT_EQ(ipv6Packet.getLayerOfType<pcpp::IPv6Layer>()->getSrcIPv6Address(), pcpp::IPv6Address(srcIpv6));
}

// For IP Layer with only destination IPs
TEST_F(CustomPacketTest, AddIPLayer_WithOnlyDstIPs)
{
    // IPv4
    customPacket.addIPLayer(std::nullopt, dstIpv4);
    customPacket.buildPacket();

    const pcpp::Packet &ipv4Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv4Packet.isPacketOfType(pcpp::IPv4));
    ASSERT_EQ(ipv4Packet.getLayerOfType<pcpp::IPv4Layer>()->getDstIPv4Address(), pcpp::IPv4Address(dstIpv4));

    // IPv6
    customPacket.addIPLayer(std::nullopt, dstIpv6);
    customPacket.buildPacket();

    const pcpp::Packet &ipv6Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv6Packet.isPacketOfType(pcpp::IPv6));
    ASSERT_EQ(ipv6Packet.getLayerOfType<pcpp::IPv6Layer>()->getDstIPv6Address(), pcpp::IPv6Address(dstIpv6));
}

// For IP Layer with no IPs
TEST_F(CustomPacketTest, AddIPLayer_WithNoIPs)
{
    customPacket.addIPLayer(std::nullopt, std::nullopt);
    customPacket.buildPacket();

    const pcpp::Packet &ipv4Packet = customPacket.getPacket();
    ASSERT_TRUE(ipv4Packet.isPacketOfType(pcpp::IPv4));
}

// For Transport Layer with all values
TEST_F(CustomPacketTest, AddTransportLayer_WithAllValues)
{
    // For TCP
    customPacket.addTransportLayer(TransportProtocol::TCP, srcPort, dstPort);
    customPacket.buildPacket();

    const pcpp::Packet &tcpPacket = customPacket.getPacket();
    ASSERT_TRUE(tcpPacket.isPacketOfType(pcpp::TCP));
    ASSERT_EQ(tcpPacket.getLayerOfType<pcpp::TcpLayer>()->getSrcPort(), srcPort);
    ASSERT_EQ(tcpPacket.getLayerOfType<pcpp::TcpLayer>()->getDstPort(), dstPort);

    // For UDP
    customPacket.addTransportLayer(TransportProtocol::UDP, srcPort, dstPort);
    customPacket.buildPacket();

    const pcpp::Packet &udpPacket = customPacket.getPacket();
    ASSERT_TRUE(udpPacket.isPacketOfType(pcpp::UDP));
    ASSERT_EQ(udpPacket.getLayerOfType<pcpp::UdpLayer>()->getSrcPort(), srcPort);
    ASSERT_EQ(udpPacket.getLayerOfType<pcpp::UdpLayer>()->getDstPort(), dstPort);
}

// For Transport Layer with only Source Port
TEST_F(CustomPacketTest, AddTransportLayer_WithOnlySrcPort)
{
    // For TCP
    customPacket.addTransportLayer(TransportProtocol::TCP, srcPort);
    customPacket.buildPacket();

    const pcpp::Packet &tcpPacket = customPacket.getPacket();
    ASSERT_TRUE(tcpPacket.isPacketOfType(pcpp::TCP));
    ASSERT_EQ(tcpPacket.getLayerOfType<pcpp::TcpLayer>()->getSrcPort(), srcPort);

    // For UDP
    customPacket.addTransportLayer(TransportProtocol::UDP, srcPort);
    customPacket.buildPacket();

    const pcpp::Packet &udpPacket = customPacket.getPacket();
    ASSERT_TRUE(udpPacket.isPacketOfType(pcpp::UDP));
    ASSERT_EQ(udpPacket.getLayerOfType<pcpp::UdpLayer>()->getSrcPort(), srcPort);
}

// For Transport Layer with only Destination Port
TEST_F(CustomPacketTest, AddTransportLayer_WithOnlyDstPort)
{
    // For TCP
    customPacket.addTransportLayer(TransportProtocol::TCP, std::nullopt, dstPort);
    customPacket.buildPacket();

    const pcpp::Packet &tcpPacket = customPacket.getPacket();
    ASSERT_TRUE(tcpPacket.isPacketOfType(pcpp::TCP));
    ASSERT_EQ(tcpPacket.getLayerOfType<pcpp::TcpLayer>()->getDstPort(), dstPort);

    // For UDP
    customPacket.addTransportLayer(TransportProtocol::UDP, std::nullopt, dstPort);
    customPacket.buildPacket();

    const pcpp::Packet &udpPacket = customPacket.getPacket();
    ASSERT_TRUE(udpPacket.isPacketOfType(pcpp::UDP));
    ASSERT_EQ(udpPacket.getLayerOfType<pcpp::UdpLayer>()->getDstPort(), dstPort);
}

// For Transport Layer with no values
TEST_F(CustomPacketTest, AddTransportLayer_WithNoValues)
{
    customPacket.addTransportLayer();
    customPacket.buildPacket();

    const pcpp::Packet &tcpPacket = customPacket.getPacket();
    ASSERT_TRUE(tcpPacket.isPacketOfType(pcpp::TCP));
}

// For Payload Layer with payload
TEST_F(CustomPacketTest, AddPayloadLayer_WithPayload)
{
    // Create packet
    customPacket.addPayloadLayer(payload);
    customPacket.buildPacket();

    // Parse packet
    const pcpp::Packet &payloadPacket = customPacket.getPacket();
    const auto *payloadLayer = payloadPacket.getLayerOfType<pcpp::PayloadLayer>();

    // Convert and compare
    std::string parsedPayload(reinterpret_cast<const char *>(payloadLayer->getPayload()), payloadLayer->getPayloadLen());
    ASSERT_EQ(parsedPayload, payload);
}

// For Payload Layer without payload
TEST_F(CustomPacketTest, AddPayloadLayer_WithNoPayload)
{
    // Create packet
    customPacket.addPayloadLayer();
    customPacket.buildPacket();

    // Parse packet
    const pcpp::Packet &payloadPacket = customPacket.getPacket();
    const auto *payloadLayer = payloadPacket.getLayerOfType<pcpp::PayloadLayer>();

    std::string parsedPayload(reinterpret_cast<const char *>(payloadLayer->getPayload()), payloadLayer->getPayloadLen());
    spdlog::info("Packet payload is: {} | Length: {}", parsedPayload, parsedPayload.size());
}