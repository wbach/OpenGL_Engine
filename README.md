# Not all project have to build in one moment on all platforms, I'm working at this.
# Wroking on linux using GCC 6.3 and Windows using MINGW32, VisualStudio 2017
# OpenGL_Engine

1) Dependencies:
    - Data, Tools:
        https://1drv.ms/f/s!At5fZiiUvIVSpHooSe4C5BqBECsS

2) Build:
    CMake possible flags:

    -DBUILD_UTILS               : "Build game engine utils"
    -DBUILD_UTILS_NETWORK       : "Build game engine network utils"
    -DBUILD_COMMON              : "Build common"
    -DBUILD_LIB_NOISE           : "Build libnoise"
    -DBUILD_GAME_ENGINE         : "Build game engine"
    -BUILD_RPG_GAME_SERVER      : "Build rpg game server"
    -DBUILD_TESTS               : "Build game engine"
    -DBUILD_TEST_GAME           : "Build test game"
    -DBUILD_TERRAIN_GENERATOR   : "Build terrain generator"

    Linux:
        Examples:
            - cmake ../Solutions/CMake/ -DBUILD_TEST_GAME=ON
            - cmake ../../Solutions/CMake/ -DBUILD_TEST_GAME=ON -DBUILD_UTILS_NETWORK=ON -DBUILD_COMMON=ON -DBUILD_RPG_GAME_SERVER=ON

    Windows:
        - MinGW
            - Download mingw and install packages.
            - Set environment variables for example :
                - CMAKE_C_COMPILER = C:\MinGW\bin\mingw32-c++.exe
                - CMAKE_CXX_COMPILER = C:\MinGW\bin\mingw32-g++.exe
                - CMAKE_MAKE_PROGRAM = C:\MinGW\bin\mingw32-make.exe
            - Add to path : 
                - C:\Program Files\CMake\bin
                - C:\MinGW\bin
            - Examples :
            - cmake -G"MinGW Makefiles" ../Solutions/CMake/ -DBUILD_TEST_GAME=ON && mingw32-make
            - cmake -G"MinGW Makefiles" -DBUILD_TEST_GAME=ON -DBUILD_UTILS_NETWORK=ON -DBUILD_COMMON=ON -DBUILD_RPG_GAME_SERVER=ON && mingw32-make

        - Visual Studio 2017
            - run ./ProjectUtils/prepareAllProjects.sh  script using for example git bash to generate VisualStudio Solution.
            - build in vs selected projects.
