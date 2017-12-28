#!/bin/bash
./getSources.sh "../../Sources/GameEngine/" GameEngineIncludes "h" "hpp"
./getSources.sh "../../Sources/GameEngine/" GameEngineSources "c" "cpp"

./getSources.sh "../../Sources/Editor/" EditorIncludes "h" "hpp"
./getSources.sh "../../Sources/Editor/" EditorSources "c" "cpp"

./getSources.sh "../../Sources/GameEngineTests/" GameEngineTestsIncludes "h" "hpp"
./getSources.sh "../../Sources/GameEngineTests/" GameEngineTestsSources "c" "cpp"

./getSources.sh "../../Sources/Gwint/" GwintIncludes "h" "hpp"
./getSources.sh "../../Sources/Gwint/" GwintSources "c" "cpp"

./getSources.sh "../../Sources/GwintServer/" GwintServerIncludes "h" "hpp"
./getSources.sh "../../Sources/GwintServer/" GwintServerSources "c" "cpp"

./getSources.sh "../../Sources/TerrainGeneration/" TerrainGenerationIncludes "h" "hpp"
./getSources.sh "../../Sources/TerrainGeneration/" TerrainGenerationSources "c" "cpp"

./getSources.sh "../../Sources/TestGame/" TestGameIncludes "h" "hpp"
./getSources.sh "../../Sources/TestGame/" TestGameSources "c" "cpp"

./getSources.sh "../../Sources/Utils/" UtilsIncludes "h" "hpp"
./getSources.sh "../../Sources/Utils/" UtilsSources "c" "cpp"

./getSources.sh "../../Sources/WindowAppCreator/" WindowAppCreatorIncludes "h" "hpp"
./getSources.sh "../../Sources/WindowAppCreator/" WindowAppCreatorSources "c" "cpp"

mv ./GameEngineIncludes.cmake ./Includes/
mv ./EditorIncludes.cmake ./Includes/
mv ./GameEngineTestsIncludes.cmake ./Includes/
mv ./GwintIncludes.cmake ./Includes/
mv ./GwintServerIncludes.cmake ./Includes/
mv ./TerrainGenerationIncludes.cmake ./Includes/
mv ./TestGameIncludes.cmake ./Includes/
mv ./UtilsIncludes.cmake ./Includes/
mv ./WindowAppCreatorIncludes.cmake ./Includes/

mv ./GameEngineSources.cmake ./Sources/
mv ./EditorSources.cmake ./Sources/
mv ./GameEngineTestsSources.cmake ./Sources/
mv ./GwintSources.cmake ./Sources/
mv ./GwintServerSources.cmake ./Sources/
mv ./TerrainGenerationSources.cmake ./Sources/
mv ./TestGameSources.cmake ./Sources/
mv ./UtilsSources.cmake ./Sources/
mv ./WindowAppCreatorSources.cmake ./Sources/

