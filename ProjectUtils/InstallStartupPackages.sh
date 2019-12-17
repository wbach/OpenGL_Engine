#!/bin/bash
#sudo usermod -a -G vboxsf baszek
sudo apt-get update -y
sudo apt-get upgrade -y
sudo apt-get install -y openssh-server
sudo apt-get install -y build-essential
sudo apt-get install -y net-tools
sudo apt-get install -y git
sudo apt-get install -y subversion
sudo apt-get install -y cmake
sudo apt-get install -y htop
sudo apt-get install -y mc
sudo apt-get install -y vim
sudo apt-get install -y dos2unix
sudo apt-get install -y gcc-multilib g++-multilib
sudo apt-get install -y libc6-dbg
sudo apt-get install -y linux-tools-common
sudo apt-get install -y libboost-all-dev
sudo apt-get install -y font-manager
sudo apt-get install -y lm-sensors
sudo apt-get install -y clang-format

sudo apt-get install -y python-pip
sudo apt-get install -y python3-pip
sudo apt-get install -y python-opencv
python -m pip install -U matplotlib
pip install -U scikit-learn
pip install tqdm
sudo apt-get install -y python-backports.functools-lru-cache
sudo apt-get install -y python-tk
pip install --upgrade google-api-python-client oauth2client

#https://github.com/prahladyeri/tuxdrive
sudo pip3 install google-api-python-client oauth2client
sudo dpkg -i tuxdrive-1.0.12.deb

#game dev
sudo apt-get install -y libxkbcommon-dev
sudo apt-get install -y libmirclient-dev
sudo apt-get install -y libgles2-mesa-dev
sudo apt-get install -y libegl1-mesa-dev
sudo apt-get install -y libsdl2-dev
sudo apt-get install -y libsdl2-net-dev
sudo apt-get install -y libglew-dev
sudo apt-get install -y libassimp-dev
sudo apt-get install -y libfreeimage3 libfreeimage-dev
sudo apt-get install -y libfreetype6-dev
sudo apt-get install -y libsdl2-ttf-dev

wget -qO - https://download.sublimetext.com/sublimehq-pub.gpg | sudo apt-key add -
sudo apt-get install apt-transport-https
echo "deb https://download.sublimetext.com/ apt/stable/" | sudo tee /etc/apt/sources.list.d/sublime-text.list
sudo apt-get update
sudo apt-get install -y sublime-text

git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf
~/.fzf/install --key-bindings --completion --update-rc

exit 1

#onde drive
sudo apt-get install -y libcurl4-openssl-dev curl git
sudo apt-get install -y libsqlite3-dev
curl -fsS https://dlang.org/install.sh | bash -s dmd
source ~/dlang/dmd-2.076.0/activate

git clone https://github.com/skilion/onedrive.git ~/onedrive
cd ~/onedrive
make
sudo -s
source ~/dlang/dmd-2.076.0/activate
make install
#exit
onedrive -m &

exit 1
#set_target_properties(UtilsLib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#add_compile_options( -std=c++14   -m32 )
sudo apt-get install -y libsdl2-dev
sudo apt-get install -y libsdl2-net-dev
sudo apt-get install -y libglew-dev
sudo apt-get install -y libassimp-dev
sudo apt-get install -y libfreeimage3 libfreeimage-dev
sudo apt-get install -y libfreetype6-dev
sudo apt-get install -y libsdl2-ttf-dev


#x86

sudo apt-get install -y libc6-dbg:i386

#game dev
sudo apt-get install -y libxkbcommon-dev:i386
sudo apt-get install -y libmirclient-dev:i386
sudo apt-get install -y libgles2-mesa-dev:i386
sudo apt-get install -y libegl1-mesa-dev:i386
sudo apt-get install -y libsdl2-dev:i386
sudo apt-get install -y libsdl2-net-dev:i386
sudo apt-get install -y libglew-dev:i386
sudo apt-get install -y libassimp-dev:i386
sudo apt-get install -y libfreeimage3:i386 libfreeimage-dev:i386
sudo apt-get install -y libfreetype6-dev:i386