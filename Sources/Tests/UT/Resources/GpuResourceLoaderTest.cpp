
#include <GameEngine/Resources/GpuResourceLoader.h>
#include <gtest/gtest.h>

#include <thread>

#include "Logger/Log.h"
#include "Resources/GpuObject.h"
#include "Types.h"

using namespace GameEngine;

class TestGpuObject : public GpuObject
{
public:
    void GpuLoadingPass() override
    {
        LOG_DEBUG << "GpuLoadingPass";
    }
    void ReleaseGpuPass() override
    {
        LOG_DEBUG << "ReleaseGpuPass";
    };
};

TEST(GpuObjectTest, MoveConstructorTransfersId)
{
    TestGpuObject obj1;
    auto id1 = obj1.GetGpuObjectId();

    TestGpuObject obj2(std::move(obj1));

    EXPECT_EQ(obj2.GetGpuObjectId(), id1) << "New object should take over the ID";
    EXPECT_EQ(obj1.GetGpuObjectId(), INVALID_ID) << "Old object should be marked as moved-from";
}

TEST(GpuObjectTest, MoveAssignmentTransfersId)
{
    TestGpuObject obj1;
    TestGpuObject obj2;

    auto id1 = obj1.GetGpuObjectId();
    auto id2 = obj2.GetGpuObjectId();

    obj2 = std::move(obj1);

    EXPECT_EQ(obj2.GetGpuObjectId(), id1) << "Target object should acquire the source's ID";
    EXPECT_EQ(obj1.GetGpuObjectId(), INVALID_ID) << "Source object should be marked as moved-from";
    EXPECT_NE(id2, obj2.GetGpuObjectId()) << "Old ID should be released";
}

TEST(GpuResourceLoaderTest, ParallelAddRemoveAndRuntimeTasks)
{
    GpuResourceLoader loader;
    std::atomic<bool> stop{false};

    std::thread gpuThread([&]() {
        while (!stop)
        {
            LOG_DEBUG << "RuntimeGpuTasks";
            loader.RuntimeGpuTasks();
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
    });

    std::thread logicThread([&]() {
        for (int i = 0; i < 5000; ++i)
        {
            auto obj = std::make_unique<TestGpuObject>();

            loader.AddObjectToGpuLoadingPass(*obj);

            if (i % 2 == 0)
                loader.AddObjectToUpdateGpuPass(*obj);

            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
            LOG_DEBUG << "AddObjectToRelease";
            loader.AddObjectToRelease(std::move(obj));
        }

        stop = true;
    });

    logicThread.join();
    gpuThread.join();

    // nie sprawdzamy ASSERT_EQ – crash = fail
    SUCCEED();
}