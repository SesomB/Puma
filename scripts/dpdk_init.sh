#!/bin/bash -e

# Function to get the list of NIC names
get_nic_names() {
    echo $(ls /sys/class/net | grep -v lo)
}

# Function to get the PCI address of a network interface
get_pci_address() {
    local iface=$1
    if [ -d "/sys/class/net/$iface/device" ]; then
        echo $(basename $(readlink /sys/class/net/$iface/device))
    else
        echo "Not a PCI device"
    fi
}

# Get the list of NIC names
NIC_NAME=$(get_nic_names)

# Get the NIC name PCI number
NIC_PCI_ADDRESS=$(get_pci_address $NIC_NAME)

echo "[-] Enabling unsafe NO-IOMMU Mode..."
echo 1 > "/sys/module/vfio/parameters/enable_unsafe_noiommu_mode"

echo "[-] Allocating hugepages..."
dpdk-hugepages.py -p 1G --setup 2G

echo "[-] DPDK Device binding..."
dpdk-devbind.py -b vfio-pci $NIC_PCI_ADDRESS

echo "[+] Done"