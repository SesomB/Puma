#include "PcapManager.hpp"

PcapManager::PcapManager(const std::string &filePath, bool write) : m_FilePath(filePath), m_Write(write), m_PcapWriter(_generateFileName()), m_PcapReader(_generateFileName())
{
    if (m_Write)
    {
        if (!m_PcapWriter.open())
        {
            spdlog::error("Unable to open PCAP file {} for writing.", filePath.c_str());
        }
    }
    else
    {
        if (!m_PcapReader.open())
        {
            spdlog::error("Unable to open PCAP file {} for reading.", filePath.c_str());
        }
    }
};

PcapManager::~PcapManager()
{
    if (m_Write)
    {
        m_PcapWriter.close();
    }
    else
    {
        m_PcapReader.close();
    }
};

bool PcapManager::writePcapFile(const std::vector<pcpp::Packet> &packets)
{
    // Validate writing mode
    if (!m_Write)
    {
        spdlog::error("Handler is not in writing mode.");
        return false;
    }

    // Write packets to Pcap file
    for (const auto &packet : packets)
    {
        m_PcapWriter.writePacket(*packet.getRawPacket());
    }

    m_PcapWriter.close();
    return true;
};

std::vector<pcpp::Packet> PcapManager::readPcapFile()
{
    std::vector<pcpp::Packet> packets;

    // Validate reading mode
    if (m_Write)
    {
        spdlog::error("Handler is not in reading mode.");
        return packets;
    }

    // Read packets
    pcpp::RawPacket rawPacket;
    while (m_PcapReader.getNextPacket(rawPacket))
    {
        pcpp::Packet parsedPacket(&rawPacket);
        packets.push_back(parsedPacket);
    }

    m_PcapReader.close();
    return packets;
};

std::string PcapManager::_generateFileName()
{
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Format timestamp
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d_%H-%M-%S");

    std::string fileName = PREFIX + ss.str() + SUFFIX;
    std::filesystem::path fullPath = std::filesystem::path(m_FilePath) / fileName;
    return fullPath.string();
};