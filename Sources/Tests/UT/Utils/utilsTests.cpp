#include <Logger/Log.h>
#include <Utils/IdPool.h>
#include <gtest/gtest.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace Utils;

TEST(StreamOperator, unorderedMapStream)
{
    std::stringstream o;
    std::unordered_map<int, std::string> map{{1, "str"}};
    o << map;

    LOG_DEBUG << map;

    std::string output = o.str();
    EXPECT_NE(output.find("1"), std::string::npos);
    EXPECT_NE(output.find("str"), std::string::npos);
}

TEST(StreamOperator, mapStream)
{
    std::stringstream o;
    std::map<int, std::string> map{{1, "str"}};
    o << map;

    LOG_DEBUG << map;

    std::string output = o.str();
    EXPECT_NE(output.find("1"), std::string::npos);
    EXPECT_NE(output.find("str"), std::string::npos);
}

TEST(StreamOperator, vectorStream)
{
    std::stringstream o;
    std::vector<int> v{1};
    o << v;

    LOG_DEBUG << v;

    std::string output = o.str();
    EXPECT_NE(output.find("1"), std::string::npos);
}

TEST(StreamOperator, setStream)
{
    std::stringstream o;
    std::set<int> v{1};
    o << v;

    LOG_DEBUG << v;

    std::string output = o.str();
    EXPECT_NE(output.find("1"), std::string::npos);
}

TEST(idPool, simpleNextId)
{
    Utils::IdPool idPool;
    EXPECT_EQ(idPool.getId(), 0);
    EXPECT_EQ(idPool.getId(), 1);
    EXPECT_EQ(idPool.getId(), 2);
}

TEST(idPool, allocateNextIdAfterSetManually)
{
    Utils::IdPool idPool;
    EXPECT_EQ(idPool.getId(), 0);
    EXPECT_EQ(idPool.getId(5), 5);
    EXPECT_EQ(idPool.getId(), 1);
    EXPECT_EQ(idPool.getId(), 2);
    EXPECT_EQ(idPool.getId(), 3);
    EXPECT_EQ(idPool.getId(), 4);
    EXPECT_EQ(idPool.getId(), 6);
}

TEST(idPool, allocateManuallyAlreadyExist)
{
    Utils::IdPool idPool;
    EXPECT_EQ(idPool.getId(), 0);
    EXPECT_EQ(idPool.getId(), 1);
    EXPECT_EQ(idPool.getId(), 2);
    EXPECT_EQ(idPool.getId(1), 3);
    EXPECT_EQ(idPool.getId(), 4);
}

TEST(idPool, nextIdAfterRemove)
{
    Utils::IdPool idPool;
    EXPECT_EQ(idPool.getId(), 0);
    EXPECT_EQ(idPool.getId(), 1);
    EXPECT_EQ(idPool.getId(), 2);
    idPool.releaseId(1);
    EXPECT_EQ(idPool.getId(), 1);
    EXPECT_EQ(idPool.getId(), 3);
}

TEST(FilesystemUtils, getRelativePathToFile_inTheSameFolder)
{
    const std::filesystem::path filepath    = "/home/baszek/path/file.txt";
    const std::filesystem::path relatiePath = "/home/baszek/path/";

    auto v = GetRelativePathToFile(relatiePath, filepath);
    EXPECT_TRUE(v.has_value());
    EXPECT_EQ(v.value(), "");
}

TEST(FilesystemUtils, getRelativePathToFile_inSubSameFolder)
{
    const std::filesystem::path filepath    = "/home/baszek/path/sub/file.txt";
    const std::filesystem::path relatiePath = "/home/baszek/path/";

    auto v = GetRelativePathToFile(relatiePath, filepath);
    EXPECT_TRUE(v.has_value());
    EXPECT_EQ(v.value(), "sub");
}

TEST(FilesystemUtils, getRelativePathToFile_inDeeperInSubSameFolder)
{
    const std::filesystem::path filepath    = "/home/baszek/path/sub/sub2/file.txt";
    const std::filesystem::path relatiePath = "/home/baszek/path/";

    auto v = GetRelativePathToFile(relatiePath, filepath);
    EXPECT_TRUE(v.has_value());
    EXPECT_EQ(v.value(), "sub/sub2");
}

TEST(FilesystemUtils, getRelativePathToFile_NotRelativeFolder)
{
    const std::filesystem::path filepath    = "/home/baszek/sub/sub2/file.txt";
    const std::filesystem::path relatiePath = "/home/baszek/path/";

    auto v = GetRelativePathToFile(relatiePath, filepath);
    EXPECT_FALSE(v.has_value());
}
