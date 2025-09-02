# Dockerfile: środowisko dev z SSH, SDL2/OpenGL, wxWidgets, fzf, distcc i pełnym home
# Użycie:
# 0. ssh-keygen -t rsa -b 4096
# 1. Zbuduj obraz: docker build --build-arg PUB_KEY="$(cat ~/.ssh/id_rsa.pub)" -t dev-env .
# 2. Uruchom kontener:
#    xhost +local:docker
#    docker run -d -p 2222:22 \
#       -e DISPLAY=$DISPLAY \
#       -v /tmp/.X11-unix:/tmp/.X11-unix \
#       --device /dev/dri \
#       -v home/$USER/Documents/GitHub/OpenGL_Engine/:/home/baszek/OpenGL_Engine \
#       --name game-engine \
#       dev-env
#    Notka: -v ścieżka/host:ścieżka/docker
# 3. Uruchom bash bezposrednio na kontenerze sudo docker exec -it game-engine /bin/bash
#    lub połącz się przez SSH: ssh -Y baszek@localhost -p 2222 (wolniejsze)

FROM ubuntu:22.04

ARG PUB_KEY

# Instalacja podstawowych narzędzi, SDL2/OpenGL, wxWidgets, fzf, distcc
#RUN sudo add-apt-repository ppa:hrzhu/wxwidgets3.2-backport
RUN apt-get update && apt-get install -y \
    openssh-server \
    build-essential \
    cmake \
    htop \
    git \
    sudo \
    mc \
    gdb \
    vim \
    libxkbcommon-dev \
    libgles2-mesa-dev \
    libegl1-mesa-dev \
    libsdl2-dev \
    libsdl2-net-dev \
    libglew-dev \
    libassimp-dev \
    libfreeimage3 \
    libfreeimage-dev \
    libfreetype6-dev \
    libsdl2-ttf-dev \
    pkg-config \
    libz-dev \
    minizip \
    libdrm-dev \
    libpulse0 \
    libstdc++-12-dev \
    clang \
    ninja-build \
    gcc-mingw-w64 \
    mingw-w64 \
    clang-format \
    libgtk-3-dev \
    libglu1-mesa-dev \
    libgl1-mesa-dev \
    libpng-dev \
    libjpeg-dev \
    libtiff-dev \
    libexpat1-dev \
    libx11-dev \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    xauth \
    distcc \
    libwxgtk3.0-gtk3-dev \
    wget \
    clangd \
    && rm -rf /var/lib/apt/lists/*

# Konfiguracja SSH i generowanie host keys
RUN mkdir /var/run/sshd && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config && \
    ssh-keygen -A

EXPOSE 22

# Tworzenie użytkownika
RUN useradd -m -s /bin/bash baszek && echo "baszek:pass" | chpasswd && adduser baszek sudo

# Pełny home użytkownika, aliasy, historia, PATH distcc
RUN mkdir -p /home/baszek/.ssh && \
    echo "$PUB_KEY" >> /home/baszek/.ssh/authorized_keys && \
    chmod 700 /home/baszek/.ssh && chmod 600 /home/baszek/.ssh/authorized_keys && \
    echo 'export PS1="[\u@\h \W]$ "' >> /home/baszek/.bashrc && \
    echo "alias ll='ls -alF'" >> /home/baszek/.bashrc && \
    echo "alias kop='grep --color=auto ./ -irHne'" >> /home/baszek/.bashrc && \
    echo "alias szukaj='find . -iname'" >> /home/baszek/.bashrc && \
    echo "export HISTFILESIZE=-1" >> /home/baszek/.bashrc && \
    echo "export HISTSIZE=-1" >> /home/baszek/.bashrc && \
    echo "export HISTFILE=/home/baszek/.bash_eternal_history" >> /home/baszek/.bashrc && \
    echo 'PROMPT_COMMAND="history -a; $PROMPT_COMMAND"' >> /home/baszek/.bashrc && \
    echo 'export PATH="/usr/lib/distcc:$PATH"' >> /home/baszek/.bashrc

RUN chown -R baszek:baszek /home/baszek

# Ustawienie katalogu roboczego
WORKDIR /home/baszek
USER baszek

# Klonowanie fzf i instalacja
RUN git clone --depth 1 https://github.com/junegunn/fzf.git /home/baszek/.fzf && \
    /home/baszek/.fzf/install --all --no-bash --no-fish --no-zsh

USER root

# Domyślne polecenie – uruchomienie SSH
CMD ["/usr/sbin/sshd", "-D"]
