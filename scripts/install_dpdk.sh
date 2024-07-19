#!/bin/bash -e

# Check if DPDK 21.11 is Installed
VERSION="21.11"
DPDK="dpdk"
DPDK_VERSION="$DPDK-$VERSION"

# Get DPDK full path
DPDK_DIR=$(find $HOME -type d -name "$DPDK_VERSION")

echo "[+] Checking if $DPDK_VERSION is already installed..."
if [ -d "$DPDK_DIR" ]; then
    echo "[-] DPDK $DPDK_VERSION is already installed."
    exit 0
fi

# Adjust DPDK full path
if [ -z "$DPDK_DIR" ]; then
    DPDK_DIR=$(find $HOME -type d -name "$DPDK")
fi

# Update and Upgrade
sudo apt update
sudo apt upgrade -y 

# Install Prerequisites
echo "[+] Step 2: Installing prerequisites..."
sudo apt install build-essential linux-headers-$(uname -r) pkg-config libnuma-dev meson ninja-build python3-pyelftools libpcap-dev -y

# Download DPDK (if not already downloaded)
DPDK_TARBALL="$DPDK_VERSION.tar.xz"
echo "[+] Downloading $DPDK_VERSION (if not already downloaded)..."
if [ ! -f "$DPDK_TARBALL" ]; then
    cd $DPDK_DIR
    wget https://fast.dpdk.org/rel/$DPDK_VERSION.tar.xz
fi

# Step 5: Extract the DPDK Archive
echo "[+] Extracting the DPDK archive..."
tar -xf $DPDK_TARBALL
cd $DPDK_VERSION

echo "Checking if DPDK is already installed..."
if command -v dpdk-testpmd &> /dev/null; then
    echo "[+] DPDK is already installed."
else
    echo "[+] Building and installing DPDK..."
    meson build
    cd build
    ninja
    sudo ninja install
fi

# Load UIO Kernel Module
echo "[+] Loading UIO kernel module..."
sudo modprobe uio

# Remove the DPDK tar.xz file
echo "[+] Removing the DPDK tar.xz file..."
rm -f $DPDK_TARBALL

echo "[+] DPDK $DPDK_VERSION installation completed."