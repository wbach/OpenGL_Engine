#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Resources/IGpuResourceLoader.h"

namespace GameEngine
{
class GpuResourceLoaderMock : public IGpuResourceLoader
{
public:
    MOCK_METHOD1(AddFunctionToCall, void(std::function<void()>));
    MOCK_METHOD0(CallFunctions, void());
    MOCK_METHOD1(AddObjectToGpuLoadingPass, void(GpuObject&));
    MOCK_METHOD0(GetObjectToGpuLoadingPass, GpuObject*());

    MOCK_METHOD1(AddObjectToUpdateGpuPass, void(GpuObject&));
    MOCK_METHOD0(GetObjectToUpdateGpuPass, GpuObject*());

    MOCK_METHOD1(AddObjectToRelease, void(std::unique_ptr<GpuObject>));
    MOCK_METHOD0(GetObjectToRelease, std::unique_ptr<GpuObject>());

    MOCK_METHOD(void, RuntimeGpuTasks, (), (override));
    MOCK_METHOD(size_t, CountObjectsToAdd, (), (override));
    MOCK_METHOD(size_t, CountObjectsToUpdate, (), (override));
    MOCK_METHOD(size_t, CountObjectsToRelease, (), (override));
};
}  // namespace GameEngine
