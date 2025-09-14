
#include <GameEngine/Resources/GpuResourceLoader.h>
#include <gtest/gtest.h>

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

TEST(GpuResourceLoaderTest, ParallelAddRemoveAndRuntimeTasks)
{
    GpuResourceLoader loader;
    std::atomic<bool> stop{false};

    std::thread gpuThread(
        [&]()
        {
            while (!stop)
            {
                loader.RuntimeGpuTasks();
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
        });

    std::thread logicThread(
        [&]()
        {
            for (int i = 0; i < 5000; ++i)
            {
                auto obj = std::make_unique<TestGpuObject>();

                loader.AddObjectToGpuLoadingPass(*obj);

                if (i % 2 == 0)
                    loader.AddObjectToUpdateGpuPass(*obj);

                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100)));
                loader.AddObjectToRelease(std::move(obj));
            }

            stop = true;
        });

    logicThread.join();
    gpuThread.join();

    // nie sprawdzamy ASSERT_EQ – crash = fail
    SUCCEED();
}