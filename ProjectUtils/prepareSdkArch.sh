#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo "Script dir : "$SCRIPT_DIR

source $SCRIPT_DIR/setEnv.sh

echo "User root : "$USERROOT

sudo rm -rf $USERROOT
mkdir $USERROOT

# Create some of the basic system folder structure
# You may run into `could not find or read directory` a lot when installing
# packages. Creating the dirs is often the solution.
mkdir "$USERROOT/etc"
mkdir -p $USERROOT/var/lib/pacman

# Copy the pacman config file from the original system
pacman_conf="$USERROOT/etc/pacman.conf"
cp /etc/pacman.conf "$pacman_conf"

# Create the keyring directory
gpgdir="$USERROOT/etc/pacman.d/gnupg"
mkdir -p "$gpgdir"

# Initialize the keyring
sudo pacman-key --gpgdir "$gpgdir" --conf "$pacman_conf" --init

# Populate the keyring
sudo pacman-key --gpgdir "$gpgdir" --conf "$pacman_conf" --populate archlinux

pacman_="sudo pacman -r $USERROOT --gpgdir $USERROOT/etc/pacman.d/gnupg"

yes | $pacman_ -Sy
yes | $pacman_ -Sy coreutils tar less findutils diffutils grep sed gawk util-linux
procps-ng

$pacman_ -S --needed --noconfirm base-devel
yes | $pacman_ -Sy sdl2
yes | $pacman_ -Sy git
yes | $pacman_ -Sy cmake
yes | $pacman_ -Sy sdl2_net
yes | $pacman_ -Sy glibc
yes | $pacman_ -Sy glew
yes | $pacman_ -Sy sdl2_ttf
yes | $pacman_ -Sy freetype2
yes | $pacman_ -Sy zlib
yes | $pacman_ -Sy minizip
yes | $pacman_ -Sy freeimage
