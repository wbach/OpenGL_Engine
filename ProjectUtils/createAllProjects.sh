#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

#GUID (or UUID) genreator: https://www.guidgenerator.com/online-guid-generator.aspx
DirectXApiId='{e2cc3475-9000-4c81-873f-fd93d925e34a}'
GraphicsApiId='{c84bf5e8-9a00-48fa-8465-4f55bbeaa7d6}'
InputId='{d9d08fe8-54bb-421a-a4a3-278046c6d26d}'
openGlApiId='{930099c9-a434-463e-b8f5-3cdabe4866ec}'
utilsId='{1AB884C5-B769-46D2-BDD0-8CEF3AD7AEB2}'
utilsNetworkId='{9D8201EF-3E5A-44CB-95DB-97D5935EF93B}'
gameEngineId='{E17E8D53-C66F-494E-B495-00EE34A0B66D}'
RpgGameServerId='{8F388312-4ADF-43B0-8ED8-609078431905}'
TestsId='{2BE6755D-3432-439E-B1A0-D39125B16AB9}'
CommonId='{5ADAA6FD-1BB7-4369-825D-7777126B7817}'
GameId='{68764853-a2a7-4c1a-97f3-95302edac572}'
EditorId='{000f8fb9-aaa2-4c7c-8aea-9cf258537f01}'
WxEditorId='{666240cc-cdf0-45de-bfcb-d198125748fe}'

#ge='GameEngine:'$gameEngineId':'$utilsId
#./prepareSolution.sh name:id:dependID...:dependId...:..
echo "Generate VisualStudio solution"
./prepareSolution.sh 'DirectXApi':$DirectXApiId:$GraphicsApiId:$utilsId 'GraphicsApi':$GraphicsApiId::$InputId 'Input':$InputId 'OpenGLApi':$openGlApiId:$utilsId 'RpgGameServer:'$RpgGameServerId':'$utilsId:$utilsNetworkId:$CommonId 'UtilsNetwork':$utilsNetworkId:$utilsId 'Utils:'$utilsId 'GameEngine:'$gameEngineId':'$utilsId 'Game':$GameId':'$gameEngineId:$DirectXApiId:$openGlApiId:$InputId:$utilsId:$utilsNetworkId:$CommonId:$GraphicsApiId 'Editor':$EditorId':'$gameEngineId:$DirectXApiId:$openGlApiId:$InputId:$utilsId:$utilsNetworkId:$CommonId:$GraphicsApiId 'WxEditor':$WxEditorId':'$gameEngineId:$DirectXApiId:$openGlApiId:$InputId:$utilsId:$utilsNetworkId:$CommonId:$GraphicsApiId 'Tests:'$TestsId':'$gameEngineId 'Common':$CommonId:$utilsId > ../Solutions/VisualStudio/GameEngine.sln

echo "Generate VisualStudio DirectXApi project"
./prepareProject.sh DirectXApi $DirectXApiId StaticLibrary > ../Sources/DirectXApi/DirectXApi.vcxproj

echo "Generate VisualStudio OpenGLApi project"
./prepareProject.sh OpenGLApi $openGlApiId StaticLibrary > ../Sources/OpenGLApi/OpenGLApi.vcxproj

echo "Generate VisualStudio GraphicsApi project"
./prepareProject.sh GraphicsApi $GraphicsApiId StaticLibrary > ../Sources/GraphicsApi/GraphicsApi.vcxproj

echo "Generate VisualStudio Input project"
./prepareProject.sh Input $InputId StaticLibrary > ../Sources/Input/Input.vcxproj

echo "Generate VisualStudio GameEngine project"
./prepareProject.sh GameEngine $gameEngineId StaticLibrary  > ../Sources/GameEngine/GameEngine.vcxproj

echo "Generate VisualStudio Common project"
./prepareProject.sh Common $CommonId StaticLibrary Utils > ../Sources/Common/Common.vcxproj

echo "Generate VisualStudio Game project"
./prepareProject.sh Game $GameId Application GameEngine Input DirectXApi OpenGLApi GraphicsApi Utils UtilsNetwork Common > ../Sources/Game/Game.vcxproj

echo "Generate VisualStudio Editor project"
./prepareProject.sh Editor $EditorId Application GameEngine Input DirectXApi OpenGLApi GraphicsApi Utils UtilsNetwork Common > ../Sources/Editor/Editor.vcxproj

echo "Generate VisualStudio WxEditor project"
./prepareProject.sh WxEditor $WxEditorId Application GameEngine Input DirectXApi OpenGLApi GraphicsApi Utils UtilsNetwork Common > ../Sources/WxEditor/WxEditor.vcxproj

echo "Generate VisualStudio RpgGameServer project"
./prepareProject.sh RpgGameServer $testGameId Application Utils UtilsNetwork Common > ../Sources/RpgGameServer/RpgGameServer.vcxproj

echo "Generate VisualStudio Utils project"
./prepareProject.sh Utils $utilsId StaticLibrary > ../Sources/Utils/Utils.vcxproj

echo "Generate VisualStudio UtilsNetwork project"
./prepareProject.sh UtilsNetwork $utilsNetworkId StaticLibrary Utils > ../Sources/UtilsNetwork/UtilsNetwork.vcxproj

echo "Generate VisualStudio Tests project"
./prepareProject.sh Tests $TestsId Application GameEngine Input DirectXApi OpenGLApi GraphicsApi Utils UtilsNetwork Common > ../Sources/Tests/Tests.vcxproj