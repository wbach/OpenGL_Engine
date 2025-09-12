#include <GameEngine/Resources/File.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "Engine/Configuration.h"
#include "Logger/Log.h"

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

TEST(FileTests, Empty)
{
    GameEngine::File file;

    EXPECT_TRUE(file.empty());
    EXPECT_FALSE(file.exist());
    EXPECT_FALSE(file);
}

TEST(FileTests, DataPathEmpty)
{
    EngineConf.files.data = {};
    GameEngine::File file("Folder/Blabla/dummy.obj");
    EXPECT_EQ(file.GetAbsolutePath(), std::filesystem::current_path() / "Folder/Blabla/dummy.obj");
}

TEST(FileTests, DataPathIsRelative)
{
    EngineConf.files.data = "../Data";
    GameEngine::File file("Folder/Blabla/dummy.obj");
    EXPECT_EQ(file.GetAbsolutePath(), std::filesystem::current_path().parent_path() / "Data/Folder/Blabla/dummy.obj");
}

TEST(FileTests, DataPathIsRelativeEndingWithSlash)
{
    EngineConf.files.data = "../Data/";
    GameEngine::File file("Folder/Blabla/dummy.obj");
    EXPECT_EQ(file.GetAbsolutePath(), std::filesystem::current_path().parent_path() / "Data/Folder/Blabla/dummy.obj");
}


TEST(FileTests, IsFormat)
{
    GameEngine::File file("Folder/Blabla/dummy.obj");
    EXPECT_TRUE(file.IsFormat("obj"));
    EXPECT_TRUE(file.IsFormat(".obj"));
    EXPECT_TRUE(file.IsFormat("OBJ"));
    EXPECT_TRUE(file.IsFormat(".OBJ"));
}