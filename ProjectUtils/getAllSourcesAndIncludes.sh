#!/bin/bash
cd ../Solutions/CMake/

function createSrc()
{
    path='../../Sources/'$1'/'
    ../../ProjectUtils/getSources.sh $path $1'Includes' "h" "hpp"
    ../../ProjectUtils/getSources.sh $path $1'Sources' "c" "cpp"
    mv $1'Includes.cmake' ./Includes/
    mv $1'Sources.cmake' ./Sources/
}

createSrc DirectXApi
createSrc VulkanApi
createSrc GraphicsApi
createSrc Input
createSrc OpenGLApi
createSrc GameEngine
createSrc Editor
createSrc GameEngineTests
createSrc Gwint
createSrc GwintServer
createSrc TerrainGeneration
createSrc TestGame
createSrc Utils
createSrc UtilsNetwork
createSrc UtilsNetworkTests
createSrc WindowAppCreator
createSrc RpgGameServer
createSrc Common
createSrc Avatar

cd ../../ProjectUtils/