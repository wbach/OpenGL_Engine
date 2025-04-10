project(EngineSDK)
cmake_minimum_required(VERSION 3.0)

message(STATUS "Project directory : ${PROJECT_DIR}")
message(STATUS "Sdk directory : ${SDK_DIR}")

add_subdirectory(${PROJECT_DIR}/Tools/common/assimp-5.0.1/ ${SDK_DIR}/assimp)
add_subdirectory(${PROJECT_DIR}/Tools/common/bullet/ ${SDK_DIR}/bullet)