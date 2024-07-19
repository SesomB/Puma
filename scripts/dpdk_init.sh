#!/bin/bash -e

NIC_PCI_ADDRESS="02:05.0"

echo "[-] Enabling unsafe NO-IOMMU Mode..."
echo 1 > "/sys/module/vfio/parameters/enable_unsafe_noiommu_mode"

echo "[-] Allocating hugepages..."
dpdk-hugepages.py -p 1G --setup 2G

echo "[-] DPDK Device binding..."
dpdk-devbind.py -b vfio-pci $NIC_PCI_ADDRESS

echo "[+] Done"