#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "PacketManager/PacketManager.hpp"
#include "PcapManager/PcapManager.hpp"

class PcapManagerTest : public ::testing::Test
{
protected:
    CustomPacket customPacket;
    PacketManager packetManager;

    std::string mountPath = "/mnt/hgfs/shared/";

    // Define data for customPacket
    const std::string srcMac = "00:11:22:33:44:55";
    const std::string dstMac = "66:77:88:99:AA:BB";
    const std::string srcIpv4 = "192.168.1.1";
    const std::string dstIpv4 = "192.168.1.20";
    const std::string srcIpv6 = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    const std::string dstIpv6 = "2001:0db8:85a3:0000:0000:8a2e:0370:7335";
    const uint16_t srcPort = 443;
    const uint16_t dstPort = 80;
    std::string payload = "Hello World!";

    // Define data for packetManager
    int numOfPackets = 5;

    // std::vector<std::string>
    //     srcMacs = {"00:11:22:33:44:55", "00:11:22:33:44:66"};
    std::vector<std::string>
        srcMacs = {};
    std::vector<std::string> dstMacs = {"66:55:44:33:22:11"};
    std::vector<std::string> srcIps = {"192.168.1.1", "192.168.1.2", "10.0.0.6"};
    std::vector<std::string> dstIps = {"192.168.1.10", "192.168.1.20"};
    std::vector<TransportProtocol> protocols = {TransportProtocol::TCP, TransportProtocol::UDP};
    std::vector<uint16_t> srcPorts = {12345, 12346};
    std::vector<uint16_t> dstPorts = {80, 443};
    std::vector<std::string> payloads = {"Hello World!"};
};

TEST_F(PcapManagerTest, UsingCustomPacket)
{
    customPacket.addEthernetLayer(srcMac, dstMac);
    customPacket.addIPLayer(srcIpv6, dstIpv6);
    customPacket.addTransportLayer(TransportProtocol::TCP, srcPort, dstPort);
    customPacket.addPayloadLayer(payload);
    customPacket.buildPacket();

    pcpp::Packet packet = customPacket.getPacket();
    const std::vector<pcpp::Packet> &packets = {packet};
    // PcapManager(mountPath, true).writePcapFile(packets);
}

TEST_F(PcapManagerTest, UsingPacketManager)
{

    packetManager.createPackets(numOfPackets,
                                srcMacs, dstMacs, srcIps, dstIps, protocols, srcPorts, dstPorts, payloads);

    const auto &packets = packetManager.getPackets();
    PcapManager(mountPath, true).writePcapFile(packets);
}
