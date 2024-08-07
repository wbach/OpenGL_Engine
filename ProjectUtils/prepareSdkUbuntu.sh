#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo "Script dir : "$SCRIPT_DIR

source $SCRIPT_DIR/setEnv.sh

echo "User root : "$USERROOT

rm -rf $USERROOT
mkdir $USERROOT

mkdir $USERROOT/tmp
cd $USERROOT/tmp

PACKAGES="cmake pkg-config clang-format build-essential libsdl2-dev libsdl2-net-dev libglew-dev libfreeimage-dev libfreetype6-dev libsdl2-ttf-dev libz-dev minizip libdrm-dev libpulse0 libstdc++-12-dev clang ninja-build apt-get install gcc-mingw-w64 mingw-w64"
apt-get download $(apt-cache depends --recurse --no-recommends --no-suggests \
  --no-conflicts --no-breaks --no-replaces --no-enhances \
  --no-pre-depends ${PACKAGES} | grep "^\w")

for i in *deb;do dpkg --extract $i ../; done

# update-alternatives --config x86_64-w64-mingw32-g++ // chose /usr/bin/x86_64-w64-mingw32-g++-posix
# update-alternatives --config x86_64-w64-mingw32-gcc  // chose /usr/bin/x86_64-w64-mingw32-gcc-posix



