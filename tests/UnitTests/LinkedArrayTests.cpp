#include "ContiguousLinkedList/ContiguousLinkedList.hpp"
#include "DPDKAllocator/DPDKAllocator.hpp"
#include "LinkedArray/LinkedArray.hpp"

#include "gtest/gtest.h"
#include <stdio.h>

#define TABLE_HASH_BIT_COUNT 24
#define TABLE_SIZE 1 << TABLE_HASH_BIT_COUNT
#define MAX_COLLISIONS 4

// struct TrackDescriptor
// {
//     uint32_t hash;
//     uint32_t src_ip;
//     uint32_t dst_ip;
//     uint16_t src_port;
//     uint16_t dst_port;
// } __rte_packed;

// typedef std::array<LinkedArray<TrackDescriptor, MAX_COLLISIONS>, TABLE_SIZE> TrackTable;

TEST(LinkedArrayTests, tracking_table)
{
    // std::cout << "TrackDescriptor size : " << sizeof(TrackDescriptor) << "B" << std::endl;

    // std::cout << "TrackTable size : " << sizeof(TrackTable) / 1e6 << "MB"
    //           << " | " << sizeof(TrackTable) / 1e9 << "GB" << std::endl;

    // ASSERT_TRUE(true);

    // TrackTable *trackTable = new TrackTable();
    // std::cout << "TrackTable[0] size : " << sizeof(trackTable[0]) << "B" << std::endl;
    // sizeof(TrackTable);
    // trackTable->at(10).push_back({.hash = 100});

    // ASSERT_TRUE(true);

    // delete trackTable;
}

TEST(LinkedArrayTests, stl_allocated)
{
    LinkedArray<int, 10> lArray;
    lArray.push_back(0);
    lArray.push_back(1);
    lArray.push_back(2);
    lArray.push_back(3);
    lArray.push_back(4);

    ASSERT_EQ(5, lArray.size());
}
