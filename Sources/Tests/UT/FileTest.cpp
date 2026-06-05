#include <GameEngine/Resources/File.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "Engine/Configuration.h"
#include "Logger/Log.h"

using namespace GameEngine;

std::string GetTestAbsoluteDataBasePath()
{
#ifdef _WIN32
    return std::filesystem::path("C:/Users/basze/Documents/GitHub/OpenGL_Engine/Data/").lexically_normal().string();
#else
    return std::filesystem::path("/home/baszek/Projects/RPG/Data/").lexically_normal().string();
#endif
}

TEST(FileTests, DataRelativeCreation)
{
    std::string const dataBasePath = GetTestAbsoluteDataBasePath();
    EngineLocalConf.files.setDataPath(dataBasePath);
    
    GameEngine::File file("Assets/a/Monk.md5mesh");
    
    std::string const expected = (std::filesystem::path(dataBasePath) / "Assets/a/Monk.md5mesh").lexically_normal().string();

    EXPECT_EQ(file.GetAbsolutePath(), expected);
}

TEST(FileTests, AbsolutePathCreation)
{
    std::string const dataBasePath = GetTestAbsoluteDataBasePath();
    EngineLocalConf.files.setDataPath(dataBasePath);
    
    std::filesystem::path const absolutePath = std::filesystem::path(dataBasePath) / "Assets/a/Monk.md5mesh";
    GameEngine::File file(absolutePath.lexically_normal().string());
    
    std::string const expected = std::filesystem::path("Assets/a/Monk.md5mesh").lexically_normal().string();

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
    EngineLocalConf.files.setDataPath("");
    GameEngine::File file("Folder/Blabla/dummy.obj");
    EXPECT_EQ(file.GetAbsolutePath(), (std::filesystem::current_path() / "Folder/Blabla/dummy.obj").lexically_normal());
}

TEST(FileTests, DataPathIsRelative)
{
    EngineLocalConf.files.setDataPath("../Data");
    GameEngine::File file("Folder/Blabla/dummy.obj");
    EXPECT_EQ(file.GetAbsolutePath(), std::filesystem::current_path().parent_path() / "Data/Folder/Blabla/dummy.obj");
}

TEST(FileTests, DataPathIsRelativeEndingWithSlash)
{
    EngineLocalConf.files.setDataPath("../Data");
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