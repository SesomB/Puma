#pragma once

#include <map>
#include <arpa/inet.h>
#include "DPDKTimer/DPDKTimer.hpp"

struct SessionTuple
{
    std::string srcIP;
    std::string dstIP;
    uint16_t srcPort;
    uint16_t dstPort;
    std::string protocol;

    SessionTuple reversed() const
    {
        return {dstIP, srcIP, dstPort, srcPort, protocol};
    }
};

static uint32_t makeSessionKey(const SessionTuple &tuple)
{
    // Convert IP strings to 32-bit integers
    in_addr srcAddr, dstAddr;
    inet_pton(AF_INET, tuple.srcIP.c_str(), &srcAddr);
    inet_pton(AF_INET, tuple.dstIP.c_str(), &dstAddr);

    // Combine srcIP, srcPort, dstIP, and dstPort into a single uint32_t key
    uint32_t key = (srcAddr.s_addr ^ dstAddr.s_addr) ^ (tuple.srcPort ^ tuple.dstPort);

    return key;
}

class SessionContext
{

public:
    // DpdkTimer<3> mTimer;
    std::map<uint32_t, SessionTuple> mSessionMap; // <seq, packet>
    uint32_t mExpectedSeq;
    uint32_t mSessionPacketCount;

    int mExpected; // Mocks timer tick

    bool mIsFinished; // will change to true when timer ticks and map is reorderd --> use timer callback

    SessionContext(const SessionTuple &firstPacket, int expected) : mSessionPacketCount(1), mIsFinished(false), mExpectedSeq(0), mExpected(expected)
    {
        mSessionMap[mExpectedSeq++] = firstPacket;
    };

    // Mocks callback
    void callback()
    {
        if (mExpected == mSessionMap.size())
            mIsFinished = true;
    }
};