project(GameEngineCM)
cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++14 -Wall -pedantic -I/usr/include/freetype2)
INCLUDE_DIRECTORIES(
../Tools/
../Tools/rapidxml-1.13/
)

file(GLOB_RECURSE GameEngine ../GameEngine/*.cpp ../GameEngine/*.h)
file(GLOB_RECURSE TestGame ../TestGame/*.cpp ../TestGame/*.h)

add_library(GameEngineLib ${GameEngine})

add_executable(GlGame ${TestGame})
target_link_libraries(GlGame GameEngineLib SDL2 GL GLEW assimp pthread freeimage freetype GLU)
