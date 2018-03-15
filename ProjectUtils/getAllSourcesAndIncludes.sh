#!/bin/bash
cd ../Solutions/CMake/

../../ProjectUtils/getSources.sh "../../Sources/GameEngine/" GameEngineIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/GameEngine/" GameEngineSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/Editor/" EditorIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/Editor/" EditorSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/GameEngineTests/" GameEngineTestsIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/GameEngineTests/" GameEngineTestsSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/Gwint/" GwintIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/Gwint/" GwintSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/GwintServer/" GwintServerIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/GwintServer/" GwintServerSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/TerrainGeneration/" TerrainGenerationIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/TerrainGeneration/" TerrainGenerationSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/TestGame/" TestGameIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/TestGame/" TestGameSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/Utils/" UtilsIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/Utils/" UtilsSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/UtilsNetwork/" UtilsNetworkIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/UtilsNetwork/" UtilsNetworkSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/WindowAppCreator/" WindowAppCreatorIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/WindowAppCreator/" WindowAppCreatorSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/RpgGameServer/" RpgGameServerIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/RpgGameServer/" RpgGameServerSources "c" "cpp"

../../ProjectUtils/getSources.sh "../../Sources/Common/" CommonIncludes "h" "hpp"
../../ProjectUtils/getSources.sh "../../Sources/Common/" CommonSources "c" "cpp"

mv ./GameEngineIncludes.cmake ./Includes/
mv ./EditorIncludes.cmake ./Includes/
mv ./GameEngineTestsIncludes.cmake ./Includes/
mv ./GwintIncludes.cmake ./Includes/
mv ./GwintServerIncludes.cmake ./Includes/
mv ./TerrainGenerationIncludes.cmake ./Includes/
mv ./TestGameIncludes.cmake ./Includes/
mv ./UtilsIncludes.cmake ./Includes/
mv ./UtilsNetworkIncludes.cmake ./Includes/
mv ./WindowAppCreatorIncludes.cmake ./Includes/
mv ./RpgGameServerIncludes.cmake ./Includes/
mv ./CommonIncludes.cmake ./Includes/

mv ./GameEngineSources.cmake ./Sources/
mv ./EditorSources.cmake ./Sources/
mv ./GameEngineTestsSources.cmake ./Sources/
mv ./GwintSources.cmake ./Sources/
mv ./GwintServerSources.cmake ./Sources/
mv ./TerrainGenerationSources.cmake ./Sources/
mv ./TestGameSources.cmake ./Sources/
mv ./UtilsSources.cmake ./Sources/
mv ./UtilsNetworkSources.cmake ./Sources/
mv ./WindowAppCreatorSources.cmake ./Sources/
mv ./RpgGameServerSources.cmake ./Sources/
mv ./CommonSources.cmake ./Sources/

cd ../../ProjectUtils/