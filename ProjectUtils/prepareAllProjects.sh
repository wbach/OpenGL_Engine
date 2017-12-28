#!/bin/bash
utilsId='{1AB884C5-B769-46D2-BDD0-8CEF3AD7AEB2}'
gameEngineId='{E17E8D53-C66F-494E-B495-00EE34A0B66D}'
testGameId='{9B0F6058-09C8-4B74-8910-4229AA0C016E}'
GwintId='{47F36A37-C53F-45FF-93F6-2090D75680AC}'
GwintServerId='{D23B021D-1E19-47CC-B2E2-2541F678F2F4}'
GameEngineTestsId='{2BE6755D-3432-439E-B1A0-D39125B16AB9}'
TerrainGenerationId='{B89BB922-72D9-4471-825E-BFC2B53B9F4C}'

#ge='GameEngine:'$gameEngineId':'$utilsId
#./prepareSolution.sh name:id:dependID...:dependId...:..
./prepareSolution.sh 'Utils:'$utilsId 'GameEngine:'$gameEngineId':'$utilsId 'TestGame:'$testGameId':'$gameEngineId 'Gwint:'$GwintId':'$gameEngineId 'GwintServer:'$GwintServerId':'$gameEngineId 'GameEngineTests:'$GameEngineTestsId':'$gameEngineId 'TerrainGeneration:'$TerrainGenerationId > ../Solutions/VisualStudio/GameEngine.sln

./prepareProject.sh GameEngine ../Solutions/CMake/Sources/GameEngineSources.cmake ../Solutions/CMake/Includes/GameEngineIncludes.cmake $gameEngineId StaticLibrary > ../Sources/GameEngine/GameEngine.vcxproj
./prepareProject.sh TestGame ../Solutions/CMake/Sources/TestGameSources.cmake ../Solutions/CMake/Includes/TestGameIncludes.cmake $testGameId Application GameEngine Utils > ../Sources/TestGame/TestGame.vcxproj
./prepareProject.sh Gwint ../Solutions/CMake/Sources/GwintSources.cmake ../Solutions/CMake/Includes/GwintIncludes.cmake $GwintId Application GameEngine Utils > ../Sources/Gwint/Gwint.vcxproj
./prepareProject.sh GwintServer ../Solutions/CMake/Sources/GwintServerSources.cmake ../Solutions/CMake/Includes/GwintServerIncludes.cmake $GwintServerId Application GameEngine Utils > ../Sources/GwintServer/GwintServer.vcxproj
./prepareProject.sh GameEngineTests ../Solutions/CMake/Sources/GameEngineTestsSources.cmake ../Solutions/CMake/Includes/GameEngineTestsIncludes.cmake $GameEngineTestsId Application GameEngine Utils > ../Sources/GameEngineTests/GameEngineTests.vcxproj
./prepareProject.sh Utils ../Solutions/CMake/Sources/UtilsSources.cmake ../Solutions/CMake/Includes/UtilsIncludes.cmake $utilsId StaticLibrary > ../Sources/Utils/Utils.vcxproj
./prepareProject.sh TerrainGeneration ../Solutions/CMake/Sources/TerrainGenerationSources.cmake ../Solutions/CMake/Includes/TerrainGenerationIncludes.cmake $TerrainGenerationId Application > ../Sources/TerrainGeneration/TerrainGeneration.vcxproj