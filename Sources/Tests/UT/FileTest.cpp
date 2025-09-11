#include <GameEngine/Resources/File.h>
#include <gtest/gtest.h>

#include <string>

#include "Engine/Configuration.h"

using namespace GameEngine;

TEST(FileTests, DataRelativeCreation)
{
    EngineConf.files.data = "/home/baszek/Projects/RPG/Data/";
    GameEngine::File file("Assets/a/Monk.md5mesh");
    std::string expected{"/home/baszek/Projects/RPG/Data/Assets/a/Monk.md5mesh"};

    EXPECT_EQ(file.GetAbsolutePath(), expected);
}

TEST(FileTests, AbsolutePathCreation)
{
    EngineConf.files.data = "/home/baszek/Projects/RPG/Data/";
    GameEngine::File file("/home/baszek/Projects/RPG/Data/Assets/a/Monk.md5mesh");
    std::string expected{"Assets/a/Monk.md5mesh"};

    EXPECT_EQ(file.GetDataRelativePath(), expected);
}