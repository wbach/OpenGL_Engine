#!/bin/bash
./getAllSourcesAndIncludes.sh

#GUID (or UUID) genreator: https://www.guidgenerator.com/online-guid-generator.aspx
GraphicsApiId='{c84bf5e8-9a00-48fa-8465-4f55bbeaa7d6}'
InputId='{d9d08fe8-54bb-421a-a4a3-278046c6d26d}'
openGlApiId='{930099c9-a434-463e-b8f5-3cdabe4866ec}'
utilsId='{1AB884C5-B769-46D2-BDD0-8CEF3AD7AEB2}'
utilsNetworkId='{9D8201EF-3E5A-44CB-95DB-97D5935EF93B}'
gameEngineId='{E17E8D53-C66F-494E-B495-00EE34A0B66D}'
testGameId='{9B0F6058-09C8-4B74-8910-4229AA0C016E}'
RpgGameServerId='{8F388312-4ADF-43B0-8ED8-609078431905}'
GwintId='{47F36A37-C53F-45FF-93F6-2090D75680AC}'
GwintServerId='{D23B021D-1E19-47CC-B2E2-2541F678F2F4}'
GameEngineTestsId='{2BE6755D-3432-439E-B1A0-D39125B16AB9}'
TerrainGenerationId='{B89BB922-72D9-4471-825E-BFC2B53B9F4C}'
CommonId='{5ADAA6FD-1BB7-4369-825D-7777126B7817}'

#ge='GameEngine:'$gameEngineId':'$utilsId
#./prepareSolution.sh name:id:dependID...:dependId...:..
echo "Generate VisualStudio solution"
./prepareSolution.sh 'GraphicsApi':$GraphicsApiId::$InputId 'Input':$InputId 'OpenGLApi':$openGlApiId:$utilsId 'RpgGameServer:'$RpgGameServerId':'$utilsId:$utilsNetworkId:$CommonId 'UtilsNetwork':$utilsNetworkId:$utilsId 'Utils:'$utilsId 'GameEngine:'$gameEngineId':'$utilsId 'TestGame:'$testGameId':'$gameEngineId:$InputId:$utilsId:$utilsNetworkId:$CommonId 'Gwint:'$GwintId':'$gameEngineId 'GwintServer:'$GwintServerId':'$gameEngineId 'GameEngineTests:'$GameEngineTestsId':'$gameEngineId 'TerrainGeneration:'$TerrainGenerationId 'Common':$CommonId:$utilsId > ../Solutions/VisualStudio/GameEngine.sln

echo "Generate VisualStudio OpenGLApi project"
./prepareProject.sh OpenGLApi ../Solutions/CMake/Sources/OpenGLApiSources.cmake ../Solutions/CMake/Includes/OpenGLApiIncludes.cmake $openGlApiId StaticLibrary > ../Sources/OpenGLApi/OpenGLApi.vcxproj

echo "Generate VisualStudio GraphicsApi project"
./prepareProject.sh GraphicsApi ../Solutions/CMake/Sources/GraphicsApiSources.cmake ../Solutions/CMake/Includes/GraphicsApiIncludes.cmake $GraphicsApiId StaticLibrary > ../Sources/GraphicsApi/GraphicsApi.vcxproj

echo "Generate VisualStudio Input project"
./prepareProject.sh Input ../Solutions/CMake/Sources/InputSources.cmake ../Solutions/CMake/Includes/InputIncludes.cmake $InputId StaticLibrary > ../Sources/Input/Input.vcxproj

echo "Generate VisualStudio GameEngine project"
./prepareProject.sh GameEngine ../Solutions/CMake/Sources/GameEngineSources.cmake ../Solutions/CMake/Includes/GameEngineIncludes.cmake $gameEngineId StaticLibrary  > ../Sources/GameEngine/GameEngine.vcxproj

echo "Generate VisualStudio Common project"
./prepareProject.sh Common ../Solutions/CMake/Sources/CommonSources.cmake ../Solutions/CMake/Includes/CommonIncludes.cmake $CommonId StaticLibrary Utils > ../Sources/Common/Common.vcxproj

echo "Generate VisualStudio TestGame project"
./prepareProject.sh TestGame ../Solutions/CMake/Sources/TestGameSources.cmake ../Solutions/CMake/Includes/TestGameIncludes.cmake $testGameId Application GameEngine Input OpenGLApi Utils UtilsNetwork Common > ../Sources/TestGame/TestGame.vcxproj

echo "Generate VisualStudio RpgGameServer project"
./prepareProject.sh RpgGameServer ../Solutions/CMake/Sources/RpgGameServerSources.cmake ../Solutions/CMake/Includes/RpgGameServerIncludes.cmake $testGameId Application Utils UtilsNetwork Common > ../Sources/RpgGameServer/RpgGameServer.vcxproj

echo "Generate VisualStudio Gwint project"
./prepareProject.sh Gwint ../Solutions/CMake/Sources/GwintSources.cmake ../Solutions/CMake/Includes/GwintIncludes.cmake $GwintId Application GameEngine Utils > ../Sources/Gwint/Gwint.vcxproj

echo "Generate VisualStudio GwintServer project"
./prepareProject.sh GwintServer ../Solutions/CMake/Sources/GwintServerSources.cmake ../Solutions/CMake/Includes/GwintServerIncludes.cmake $GwintServerId Application GameEngine Utils > ../Sources/GwintServer/GwintServer.vcxproj

echo "Generate VisualStudio Utils project"
./prepareProject.sh Utils ../Solutions/CMake/Sources/UtilsSources.cmake ../Solutions/CMake/Includes/UtilsIncludes.cmake $utilsId StaticLibrary > ../Sources/Utils/Utils.vcxproj

echo "Generate VisualStudio UtilsNetwork project"
./prepareProject.sh UtilsNetwork ../Solutions/CMake/Sources/UtilsNetworkSources.cmake ../Solutions/CMake/Includes/UtilsNetworkIncludes.cmake $utilsNetworkId StaticLibrary Utils > ../Sources/UtilsNetwork/UtilsNetwork.vcxproj

echo "Generate VisualStudio TerrainGeneration project"
./prepareProject.sh TerrainGeneration ../Solutions/CMake/Sources/TerrainGenerationSources.cmake ../Solutions/CMake/Includes/TerrainGenerationIncludes.cmake $TerrainGenerationId Application > ../Sources/TerrainGeneration/TerrainGeneration.vcxproj

echo "Generate VisualStudio GameEngineTests project"
./prepareProject.sh GameEngineTests ../Solutions/CMake/Sources/GameEngineTestsSources.cmake ../Solutions/CMake/Includes/GameEngineTestsIncludes.cmake $GameEngineTestsId Application GameEngine Utils  UtilsNetwork Common > ../Sources/GameEngineTests/GameEngineTests.vcxproj