#include <Utils/IdPool.h>
#include <gtest/gtest.h>

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

