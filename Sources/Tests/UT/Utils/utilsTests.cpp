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

TEST(idPool, onExhaustionDoesNotReuseReleasedIdsImmediately)
{
    IdPool idPool{0, IdPool::IdReusePolicy::OnExhaustion};

    EXPECT_EQ(idPool.getId(), 0);
    EXPECT_EQ(idPool.getId(), 1);
    EXPECT_EQ(idPool.getId(), 2);

    idPool.releaseId(1);

    EXPECT_EQ(idPool.getId(), 3);
    EXPECT_EQ(idPool.getId(), 4);
}

TEST(idPool, onExhaustionReusesReleasedIdsAfterCounterExhaustion)
{
    constexpr auto maxId = std::numeric_limits<IdType>::max();

    IdPool idPool{maxId - 2, IdPool::IdReusePolicy::OnExhaustion};

    EXPECT_EQ(idPool.getId(), maxId - 2);
    EXPECT_EQ(idPool.getId(), maxId - 1);
    EXPECT_EQ(idPool.getId(), maxId);

    idPool.releaseId(maxId - 1);

    EXPECT_EQ(idPool.getId(), maxId - 1);
}

TEST(idPool, onExhaustionReusesReleasedIdsInAscendingOrder)
{
    constexpr auto maxId = std::numeric_limits<IdType>::max();

    IdPool idPool{maxId - 3, IdPool::IdReusePolicy::OnExhaustion};

    EXPECT_EQ(idPool.getId(), maxId - 3);
    EXPECT_EQ(idPool.getId(), maxId - 2);
    EXPECT_EQ(idPool.getId(), maxId - 1);
    EXPECT_EQ(idPool.getId(), maxId);

    idPool.releaseId(maxId - 1);
    idPool.releaseId(maxId - 3);
    idPool.releaseId(maxId - 2);

    EXPECT_EQ(idPool.getId(), maxId - 3);
    EXPECT_EQ(idPool.getId(), maxId - 2);
    EXPECT_EQ(idPool.getId(), maxId - 1);
}

TEST(idPool, onExhaustionRequestedIdCanUseDeferredId)
{
    constexpr auto maxId = std::numeric_limits<IdType>::max();

    IdPool idPool{maxId - 2, IdPool::IdReusePolicy::OnExhaustion};

    EXPECT_EQ(idPool.getId(), maxId - 2);
    EXPECT_EQ(idPool.getId(), maxId - 1);
    EXPECT_EQ(idPool.getId(), maxId);

    idPool.releaseId(maxId - 1);

    EXPECT_EQ(idPool.getId(maxId - 1), maxId - 1);
}

TEST(idPool, onExhaustionClearRemovesDeferredIds)
{
    constexpr auto maxId = std::numeric_limits<IdType>::max();

    IdPool idPool{maxId - 1, IdPool::IdReusePolicy::OnExhaustion};

    EXPECT_EQ(idPool.getId(), maxId - 1);
    EXPECT_EQ(idPool.getId(), maxId);

    idPool.releaseId(maxId - 1);

    idPool.clear();

    EXPECT_EQ(idPool.getId(), maxId - 1);
    EXPECT_EQ(idPool.getId(), maxId);
}

TEST(idPool, immediateRemainsDefaultPolicy)
{
    IdPool idPool;

    EXPECT_EQ(idPool.getId(), 0);
    EXPECT_EQ(idPool.getId(), 1);

    idPool.releaseId(0);

    EXPECT_EQ(idPool.getId(), 0);
}


TEST(idPool, concurrentAllocationReturnsUniqueIds)
{
    constexpr size_t threadCount = 8;
    constexpr size_t idsPerThread = 10000;
    constexpr size_t totalIds = threadCount * idsPerThread;

    IdPool idPool;

    std::vector<std::thread> threads;
    std::vector<std::vector<IdType>> results(threadCount);

    for (size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([&, i]()
        {
            results[i].reserve(idsPerThread);

            for (size_t j = 0; j < idsPerThread; ++j)
            {
                results[i].push_back(idPool.getId());
            }
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::set<IdType> uniqueIds;

    for (const auto& threadIds : results)
    {
        uniqueIds.insert(threadIds.begin(), threadIds.end());
    }

    EXPECT_EQ(uniqueIds.size(), totalIds);
    EXPECT_EQ(*uniqueIds.begin(), 0);
    EXPECT_EQ(*uniqueIds.rbegin(), totalIds - 1);
}


TEST(idPool, concurrentReleaseAndReuseDoesNotDuplicateIds)
{
    constexpr size_t threadCount = 8;
    constexpr size_t idsPerThread = 1000;
    constexpr size_t totalIds = threadCount * idsPerThread;

    constexpr auto maxId = std::numeric_limits<IdType>::max();
    constexpr auto startId = maxId - static_cast<IdType>(totalIds) + 1;

    IdPool idPool{startId, IdPool::IdReusePolicy::OnExhaustion};

    std::vector<IdType> initialIds;
    initialIds.reserve(totalIds);

    // Exhaust the ID counter.
    for (size_t i = 0; i < totalIds; ++i)
    {
        initialIds.push_back(idPool.getId());
    }

    // Release the same IDs concurrently from multiple threads.
    std::vector<std::thread> releaseThreads;

    for (size_t i = 0; i < threadCount; ++i)
    {
        releaseThreads.emplace_back([&]()
        {
            for (const auto id : initialIds)
            {
                idPool.releaseId(id);
            }
        });
    }

    for (auto& thread : releaseThreads)
    {
        thread.join();
    }

    // Reallocate all released IDs concurrently.
    std::vector<std::thread> allocationThreads;
    std::vector<std::vector<IdType>> results(threadCount);

    for (size_t i = 0; i < threadCount; ++i)
    {
        allocationThreads.emplace_back([&, i]()
        {
            results[i].reserve(idsPerThread);

            for (size_t j = 0; j < idsPerThread; ++j)
            {
                results[i].push_back(idPool.getId());
            }
        });
    }

    for (auto& thread : allocationThreads)
    {
        thread.join();
    }

    std::set<IdType> expectedIds(initialIds.begin(), initialIds.end());
    std::set<IdType> allocatedIds;

    for (const auto& threadIds : results)
    {
        allocatedIds.insert(threadIds.begin(), threadIds.end());
    }

    EXPECT_EQ(allocatedIds.size(), totalIds);
    EXPECT_EQ(allocatedIds, expectedIds);

    EXPECT_THROW(idPool.getId(), std::runtime_error);
}

