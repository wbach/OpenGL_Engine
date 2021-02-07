#include "GpuResourceLoader.h"

#include <Logger/Log.h>
#include <Types.h>
#include <algorithm>

namespace GameEngine
{
GpuResourceLoader::GpuResourceLoader()
{
    gpuPassLoad.reserve(10000);
    functions.reserve(100000);
}
void GpuResourceLoader::AddFunctionToCall(std::function<void()> f)
{
    std::lock_guard<std::mutex> lock(functionMutex);
    functions.push_back(f);
}

void GpuResourceLoader::CallFunctions()
{
    std::lock_guard<std::mutex> lock(functionMutex);
    if (functions.empty())
        return;

    functions.back()();
    functions.pop_back();
}

void GpuResourceLoader::AddObjectToGpuLoadingPass(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    gpuPassLoad.push_back(&obj);
}

GpuObject* GpuResourceLoader::GetObjectToGpuLoadingPass()
{
    if (gpuPassLoad.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(gpuPassMutex);
    GpuObject* obj = gpuPassLoad.back();
    gpuPassLoad.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToUpdateGpuPass(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(updateMutex);

    if (not objectsToUpdate.empty())
    {
        auto iter = std::find_if(
            objectsToUpdate.begin(), objectsToUpdate.end(),
            [id = obj.GetGpuObjectId()](const auto& gpuObject) { return id == gpuObject->GetGpuObjectId(); });

        if (iter == objectsToUpdate.end())
            objectsToUpdate.push_back(&obj);
    }
    else
    {
        objectsToUpdate.push_back(&obj);
    }
}

GpuObject* GpuResourceLoader::GetObjectToUpdateGpuPass()
{
    if (objectsToUpdate.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(updateMutex);
    GpuObject* obj = objectsToUpdate.back();
    objectsToUpdate.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToRelease(std::unique_ptr<GpuObject> object)
{
    IsRemoveObjectIfIsToLoadState(*object);
    IsRemoveObjectIfIsToUpdateState(*object);

    std::lock_guard<std::mutex> lock(releaseMutex);
    objectsToRelease.push_back(std::move(object));
}
std::unique_ptr<GpuObject> GpuResourceLoader::GetObjectToRelease()
{
    if (objectsToRelease.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(releaseMutex);
    auto object = std::move(objectsToRelease.back());
    objectsToRelease.pop_back();
    return object;
}
void GpuResourceLoader::IsRemoveObjectIfIsToUpdateState(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(updateMutex);

    if (not objectsToUpdate.empty())
    {
        auto iter = std::find_if(
            objectsToUpdate.begin(), objectsToUpdate.end(),
            [id = obj.GetGpuObjectId()](const auto& gpuObject) { return id == gpuObject->GetGpuObjectId(); });
        if (iter != objectsToUpdate.end())
            objectsToUpdate.erase(iter);
    }
}
void GpuResourceLoader::IsRemoveObjectIfIsToLoadState(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);

    if (not gpuPassLoad.empty())
    {
        auto iter = std::find_if(
            gpuPassLoad.begin(), gpuPassLoad.end(),
            [id = obj.GetGpuObjectId()](const auto& gpuObject) { return id == gpuObject->GetGpuObjectId(); });
        if (iter != gpuPassLoad.end())
            gpuPassLoad.erase(iter);
    }
}

void GpuResourceLoader::RuntimeGpuTasks()
{
    RuntimeReleaseObjectGpu();
    RuntimeLoadObjectToGpu();
    RuntimeUpdateObjectGpu();
    CallFunctions();
}

size_t GpuResourceLoader::CountObjectsToAdd()
{
    return gpuPassLoad.size();
}

size_t GpuResourceLoader::CountObjectsToUpdate()
{
    return objectsToUpdate.size();
}

size_t GpuResourceLoader::CountObjectsToRelease()
{
    return objectsToRelease.size();
}

void GpuResourceLoader::RuntimeLoadObjectToGpu()
{
    auto obj = GetObjectToGpuLoadingPass();

    while (obj)
    {
        if (not obj->GetGraphicsObjectId())
        {
            obj->GpuLoadingPass();
        }
        else
        {
            DEBUG_LOG("Is already loaded.");
        }

        obj = GetObjectToGpuLoadingPass();
    }
}

void GpuResourceLoader::RuntimeUpdateObjectGpu()
{
    auto obj = GetObjectToUpdateGpuPass();

    while (obj)
    {
        if (obj->GetGraphicsObjectId())
        {
            obj->UpdateGpuPass();
        }
        else
        {
            ERROR_LOG("Object not loaded");
        }

        obj = GetObjectToUpdateGpuPass();
    }
}

void GpuResourceLoader::RuntimeReleaseObjectGpu()
{
    auto obj = GetObjectToRelease();

    while (obj)
    {
        if (obj->GetGraphicsObjectId())
        {
            obj->ReleaseGpuPass();
        }
        obj = GetObjectToRelease();
    }
}
}  // namespace GameEngine
