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
    MOCK_METHOD1(AddObjectToGpuLoadingPass, void(GpuObject*));
    MOCK_METHOD0(GetObjectToGpuLoadingPass, GpuObject*());
    MOCK_METHOD1(AddObjectToGpuPostLoadingPass, void(GpuObject*));
    MOCK_METHOD0(GetObjectToGpuPostLoadingPass, GpuObject*());
};
}  // namespace GameEngine
