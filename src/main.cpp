#include "spdlog/spdlog.h"
#include <iostream>
#include <rte_eal.h>

int main(int argc, char **argv)
{
    int ret = rte_eal_init(argc, argv);

    spdlog::info("Hello");
    std::cout << ret << std::endl;
    return rte_eal_cleanup();
}