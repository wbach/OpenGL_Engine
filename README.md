1) Dependencies:
    - Data, Tools:
        https://1drv.ms/f/s!At5fZiiUvIVSpHooSe4C5BqBECsS

2) Build:
    CMake possible flags:

    -DBUILD_UTILS               : "Build game engine utils"
    -DBUILD_UTILS_NETWORK       : "Build network utils"
    -DBUILD_UTILS_NETWORK_TESTS : "Build network utils tests"
    -DBUILD_COMMON              : "Build common"
    -DBUILD_LIB_NOISE           : "Build libnoise"
    -DBUILD_GAME_ENGINE         : "Build game engine"
    -BUILD_RPG_GAME_SERVER      : "Build rpg game server"
    -DBUILD_TESTS               : "Build game engine tests"
    -DBUILD_TEST_GAME           : "Build test game"
    -DBUILD_TERRAIN_GENERATOR   : "Build terrain generator"

    Linux:
        Examples:
            - cmake -G Ninja ../Solutions/CMake -DBUILD_AVATAR_GAME=ON -DBUILD_EDITOR=ON -DBUILD_TESTS=ON 
            - cmake -G Ninja ../Solutions/CMake -DBUILD_AVATAR_GAME=ON -DBUILD_EDITOR=ON -DBUILD_TESTS=ON -DCMAKE_C_COMPILER_LAUNCHER="distcc" -DCMAKE_CXX_COMPILER_LAUNCHER="distcc"
            - cmake -G Ninja ../Solutions/CMake -DBUILD_AVATAR_GAME=ON -DBUILD_EDITOR=ON -DBUILD_TESTS=ON -DCMAKE_TOOLCHAIN_FILE=../Solutions/CMake/mingw-w64-x86_64.cmake
        
        Mingw example dll location:
        /usr/lib/gcc/x86_64-w64-mingw32/10-posix/*.dll  or -static-libgcc -static-libstdc++
        MESA_GLSL_VERSION_OVERRIDE=450 MESA_GL_VERSION_OVERRIDE=4.5 ./TestGlGame
    Windows:
        - MinGW
            - Download mingw and install packages. (https://winlibs.com/)
            - Set environment variables for example :
                - CMAKE_C_COMPILER = C:\MinGW\bin\mingw32-c++.exe
                - CMAKE_CXX_COMPILER = C:\MinGW\bin\mingw32-g++.exe
                - CMAKE_MAKE_PROGRAM = C:\MinGW\bin\mingw32-make.exe
            - Add to path : 
                - C:\Program Files\CMake\bin
                - C:\MinGW\bin
            - Examples :
            - cmake -G"MinGW Makefiles" -DBUILD_AVATAR_GAME=ON -DBUILD_EDITOR=ON -DBUILD_TESTS=ON && cmake --build . -j16
        - Visual Studio 2017
            - run ./ProjectUtils/prepareAllProjects.sh  script using for example git bash to generate VisualStudio Solution.
            - build in vs selected projects.
