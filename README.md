# Not all project have to build in one moment on all platforms, I'm working at this.
# Wroking on linux using GCC 6.3 and Windows using MINGW32, VisualStudio 2017
# OpenGL_Engine

1) Dependencies:
	- Tools (libs common, glm, rapid, glew..., mainly for for windows):
		https://drive.google.com/open?id=1QI00bce92CZ5PbOpEcrqgoSBLB2rAWZ-
	- Data:
		https://drive.google.com/open?id=0B30fieNBBvnFTks1ZmJ6QVpxRUU
		
2) Build:

	CMake possible flags:
	
	-DBUILD_UTILS 				: "Build game engine utils"
	-DBUILD_LIB_NOISE 			: "Build libnoise"
	-DBUILD_GAME_ENGINE 		: "Build game engine" ON
	-DBUILD_TESTS 				: "Build game engine" OFF
	-DBUILD_TEST_GAME 			: "Build test game"
	-DBUILD_TERRAIN_GENERATOR 	: "Build terrain generator"

	Linux:
		example:
			cmake ../Solutions/CMake/ -DBUILD_TEST_GAME=ON

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
			- cmake -G"MinGW Makefiles" ../Solutions/CMake/ -DBUILD_TEST_GAME=ON && mingw32-make

		- Visual Studio 2017
			- run ./ProjectUtils/prepareAllProjects.sh  script using for example git bash to generate VisualStudio Solution.
			- build in vs selected projects.