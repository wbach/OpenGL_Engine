#!/bin/bash
ScriptDir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ParentDir="$(dirname "$ScriptDir")"

cd $ParentDir/Solutions/CMake/

function createSrc()
{
    path='../../Sources/'$1'/'
    ../../ProjectUtils/getSources.sh $path $1'Includes' "h" "hpp"
    ../../ProjectUtils/getSources.sh $path $1'Sources' "c" "cpp"
    mv $1'Includes.cmake' ./Includes/
    mv $1'Sources.cmake' ./Sources/
}

createSrc Common
createSrc RpgGameServer
createSrc Utils
createSrc Input
createSrc UtilsNetwork
createSrc GraphicsApi
createSrc SdlApi
createSrc DirectXApi
createSrc OpenGLApi
createSrc VulkanApi
createSrc GameEngine
createSrc WxEditor
createSrc Game
createSrc Tests
createSrc UtilsNetworkTests
createSrc Benchmark

cd ../../ProjectUtils/