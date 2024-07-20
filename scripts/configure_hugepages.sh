#!/bin/bash -e

# Constants
HUGEPAGE_SIZE="1G"
NUM_HUGEPAGES=4
VERSION="21.11"
DPDK="dpdk"
DPDK_VERSION="$DPDK-$VERSION"

# Get DPDK full path
DPDK_DIR=$(find $HOME -type d -name "$DPDK_VERSION")

# Function to configure hugepages at boot time
configure_hugepages_boot() {
    echo "[+] Configuring hugepages at boot time..."

    # Edit GRUB configuration
    sudo sed -i "s/GRUB_CMDLINE_LINUX_DEFAULT=\"[^\"]*/& default_hugepagesz=$HUGEPAGE_SIZE hugepagesz=$HUGEPAGE_SIZE hugepages=$NUM_HUGEPAGES/" /etc/default/grub
    
    # Update GRUB
    echo "[+] Updating GRUB..."
    sudo update-grub
    
    # Reboot the system
    echo "[+] Please reboot the system for the changes to take effect."
}

# Function to configure hugepages dynamically
configure_hugepages_dynamic() {
    echo "[+] Configuring hugepages dynamically..."

    # Allocate hugepages
    echo $NUM_HUGEPAGES | sudo tee /sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages

    # Verify allocation
    ALLOCATED_HUGEPAGES=$(cat /sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages)
    if [ "$ALLOCATED_HUGEPAGES" -eq "$NUM_HUGEPAGES" ]; then
        echo "Successfully allocated $ALLOCATED_HUGEPAGES hugepages."
    else
        echo "[!] Error: Unable to allocate the required number of hugepages."
        echo "Hugepages allocated: $ALLOCATED_HUGEPAGES"
        echo "Hugepages requested: $NUM_HUGEPAGES"
        exit 1
    fi

    echo "Hugepages allocation details:"
    grep Huge /proc/meminfo
}

# Function to mount the hugepages file system
mount_hugepages() {
    echo "[+] Mounting the hugepages file system..."

    # Create mount point
    sudo mkdir -p /mnt/huge

    # Mount hugepages file system
    sudo mount -t hugetlbfs nodev /mnt/huge

    # Verify the mount
    echo "[+] Verifying the mount..."
    mount | grep huge
}

setup_environment() {
    echo "[+] Setting up environment variables..."

    # Add environment variables to .bashrc
    echo 'export RTE_SDK=$DPDK_DIR' >> ~/.bashrc
    echo 'export RTE_TARGET=x86_64-native-linux-gcc' >> ~/.bashrc
    echo 'export HUGEPAGES_PATH=/mnt/huge' >> ~/.bashrc

    # Source .bashrc to apply changes
    source ~/.bashrc
}

# Main script execution
main() {
    echo "Reserving Hugepages for DPDK Use"

    # Configure hugepages at boot time
    configure_hugepages_boot

    # Configure hugepages dynamically
    configure_hugepages_dynamic

    # Mount the hugepages file system
    mount_hugepages

    # Set up environment variables
    setup_environment

    echo "[+] Hugepages configuration completed. Please reboot your system for the changes to take effect."
}

main