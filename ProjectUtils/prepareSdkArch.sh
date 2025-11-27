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
sudo pacman-key --gpgdir "$gpgdir" --conf "$pacman_conf" --populate holo

pacman_="sudo pacman -r $USERROOT --gpgdir $USERROOT/etc/pacman.d/gnupg"


echo "Init..."
yes | $pacman_ -Sy
yes | $pacman_ -S coreutils tar less findutils diffutils grep sed gawk util-linux procps-ng


echo "Instaling..."
#$pacman_ -S --needed --noconfirm base-devel
yes | $pacman_ -Sy base-devel pkg-config sdl2 git cmake sdl2_net glew sdl2_ttf freetype2 zlib minizip freeimage jsoncpp rhash libuv cppdap gcc make libmpc libisl glfw-x11 glu libgl libpng harfbuzz brotli glib2 libsysprof-capture pcre2 graphite libc++ linux-headers linux-api-headers jxrlib glibc distcc ninja wxwidgets-gtk3



cat > $USERROOT/etc/distcc/hosts <<- EOM
# --- /etc/distcc/hosts -----------------------
# See the "Hosts Specification" section of
# "man distcc" for the format of this file.
# By default, just test that it works in loopback mode.
192.168.0.35/12
EOM
sudo cp -r $USERROOT/etc/distcc/ /etc/
