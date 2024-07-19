set(PCAPPP_USE_DPDK ON CACHE BOOL "Use DPDK" FORCE)
add_subdirectory(argparse)
add_subdirectory(PcapPlusPlus)
add_subdirectory(spdlog)