#!/bin/bash
# ============================================
# Captain Tsubasa II - HD Remake
# Full build script (Linux/Ubuntu)
# ============================================

set -e

echo "================================================"
echo " Captain Tsubasa II HD Remake - Build Script"
echo "================================================"

# ---- STEP 1: Install dependencies ----
echo "[1/5] Installing dependencies..."
sudo apt-get update -qq
sudo apt-get install -y \
    git build-essential cmake \
    genisoimage \
    texinfo bison flex \
    libgmp-dev libmpfr-dev libmpc-dev

# ---- STEP 2: Install PS2DEV toolchain ----
if [ ! -d "$HOME/ps2dev" ]; then
    echo "[2/5] Building PS2DEV toolchain (ໃຊ້ເວລາ 20-40 ນາທີ)..."
    cd /tmp
    git clone https://github.com/ps2dev/ps2dev.git
    cd ps2dev
    export PS2DEV=/usr/local/ps2dev
    export PS2SDK=$PS2DEV/ps2sdk
    export PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin
    sudo ./setup_ci.sh
else
    echo "[2/5] PS2DEV already installed, skipping..."
fi

# ---- STEP 3: Set environment ----
echo "[3/5] Setting environment..."
export PS2DEV=/usr/local/ps2dev
export PS2SDK=$PS2DEV/ps2sdk
export PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin

# Add to .bashrc for future sessions
if ! grep -q "PS2DEV" ~/.bashrc; then
    echo 'export PS2DEV=/usr/local/ps2dev' >> ~/.bashrc
    echo 'export PS2SDK=$PS2DEV/ps2sdk'   >> ~/.bashrc
    echo 'export PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin' >> ~/.bashrc
fi

# ---- STEP 4: Compile ----
echo "[4/5] Compiling..."
cd "$(dirname "$0")"
make clean
make

echo "ELF created: CaptainTsubasa2.elf"

# ---- STEP 5: Build ISO ----
echo "[5/5] Building ISO..."
make iso

echo ""
echo "================================================"
echo " BUILD COMPLETE!"
echo " File: CaptainTsubasa2.iso"
echo ""
echo " ວິທີຫຼິ້ນ:"
echo " 1. ໂຫຼດ PCSX2 emulator"
echo " 2. File > Boot ISO"
echo " 3. ເລືອກ CaptainTsubasa2.iso"
echo "================================================"
