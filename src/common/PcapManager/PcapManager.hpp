#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <Packet.h>
#include <PcapFileDevice.h>
#include "spdlog/spdlog.h"

const std::string PREFIX = "PcapManager_";
const std::string SUFFIX = ".pcap";

class PcapManager
{
private:
    std::string m_FilePath;
    pcpp::PcapFileWriterDevice m_PcapWriter;
    pcpp::PcapFileReaderDevice m_PcapReader;
    bool m_Write;

public:
    PcapManager(const std::string &filePath, bool write = true);
    ~PcapManager();

    bool writePcapFile(const std::vector<pcpp::Packet> &packets);

    std::vector<pcpp::Packet> readPcapFile();

private:
    std::string _generateFileName();
};