#pragma once
#include <gmock/gmock.h>

#include "GameEngine/Resources/IGpuResourceLoader.h"

namespace GameEngine
{
class GpuResourceLoaderMock : public IGpuResourceLoader
{
public:
    MOCK_METHOD1(AddFunctionToCall, void(std::function<void()>));
    MOCK_METHOD1(AddObjectToGpuLoadingPass, void(GpuObject&));

    MOCK_METHOD1(AddObjectToUpdateGpuPass, void(GpuObject&));

    MOCK_METHOD(void, AddObjectToRelease, (std::unique_ptr<GpuObject>), (override));
    MOCK_METHOD(void, AddObjectsToRelease, (std::vector<std::unique_ptr<GpuObject>>&&), (override));

    MOCK_METHOD(void, RuntimeGpuTasks, (), (override));
    MOCK_METHOD(size_t, CountObjectsInQueues, (), (const override));
};
}  // namespace GameEngine
