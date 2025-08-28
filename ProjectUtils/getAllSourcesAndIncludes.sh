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
createSrc DirectXApi
createSrc Editor
createSrc Game
createSrc GameEngine
createSrc GraphicsApi
createSrc Input
createSrc OpenGLApi
createSrc RpgGameServer
createSrc Tests
createSrc Utils
createSrc UtilsNetwork
createSrc UtilsNetworkTests
createSrc WxEditor

cd ../../ProjectUtils/