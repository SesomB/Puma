#include "ContiguousLinkedList/ContiguousLinkedList.hpp"
#include "DPDKAllocator/DPDKAllocator.hpp"

#include "gtest/gtest.h"
#include <stdio.h>

TEST(ContiguousLinkedListTest, stl_allocated)
{
    ContiguousLinkedList<int> list(10);

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_front(0);

    ASSERT_EQ(list.size(), 4);
    list.pop_front();
    list.pop_back();

    ASSERT_EQ(list.size(), 2);

    auto it = list.begin();
    ++it;
    list.insert(it, 42);

    ASSERT_EQ(list.size(), 3);

    list.erase(it);
    ASSERT_EQ(list.size(), 2);
}

TEST(ContiguousLinkedListTest, dpdk_allocated)
{
    ContiguousLinkedList<int, DPDKAllocator<int>> list(10);

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_front(0);

    std::cout << "List after push operations: ";
    list.print_list();

    list.pop_front();
    list.pop_back();

    std::cout << "List after pop operations: ";
    list.print_list();

    auto it = list.begin();
    ++it;
    list.insert(it, 42);

    std::cout << "List after insertion: ";
    list.print_list();

    list.erase(it);

    std::cout << "List after erasure: ";
    list.print_list();
}
