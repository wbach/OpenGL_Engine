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

GpuResourceLoader::~GpuResourceLoader()
{
    LOG_DEBUG << "~GpuResourceLoader";
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

    auto f = std::move(functions.back());
    functions.pop_back();
    f();
}

void GpuResourceLoader::AddObjectToGpuLoadingPass(GpuObject& obj)
{
    LOG_DEBUG << "AddObjectToGpuLoadingPass :" << obj.GetGpuObjectId();
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    gpuPassLoad.push_back(&obj);
}

GpuObject* GpuResourceLoader::GetObjectToGpuLoadingPass()
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);

    if (gpuPassLoad.empty())
        return nullptr;

    GpuObject* obj = gpuPassLoad.back();
    gpuPassLoad.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToUpdateGpuPass(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(updateMutex);

    if (not objectsToUpdate.empty())
    {
        auto iter =
            std::find_if(objectsToUpdate.begin(), objectsToUpdate.end(),
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
    std::lock_guard<std::mutex> lock(updateMutex);
    if (objectsToUpdate.empty())
        return nullptr;

    GpuObject* obj = objectsToUpdate.back();
    objectsToUpdate.pop_back();
    return obj;
}

void GpuResourceLoader::AddObjectToRelease(std::unique_ptr<GpuObject> object)
{
    if (not object)
        return;

    std::scoped_lock lock(gpuPassMutex, updateMutex, releaseMutex);

    RemoveObjectIfIsToLoadState(*object);
    RemoveObjectIfIsToUpdateState(*object);

    objectsToRelease.push_back(std::move(object));
}
std::unique_ptr<GpuObject> GpuResourceLoader::GetObjectToRelease()
{
    std::lock_guard<std::mutex> lock(releaseMutex);
    if (objectsToRelease.empty())
        return nullptr;

    auto object = std::move(objectsToRelease.back());
    objectsToRelease.pop_back();
    return object;
}
void GpuResourceLoader::RemoveObjectIfIsToUpdateState(GpuObject& obj)
{
    if (not objectsToUpdate.empty())
    {
        auto iter =
            std::find_if(objectsToUpdate.begin(), objectsToUpdate.end(),
                         [id = obj.GetGpuObjectId()](const auto& gpuObject) { return id == gpuObject->GetGpuObjectId(); });
        if (iter != objectsToUpdate.end())
            objectsToUpdate.erase(iter);
    }
}
void GpuResourceLoader::RemoveObjectIfIsToLoadState(GpuObject& obj)
{
    if (not gpuPassLoad.empty())
    {
        auto iter = std::find_if(gpuPassLoad.begin(), gpuPassLoad.end(), [id = obj.GetGpuObjectId()](const auto& gpuObject) {
            return id == gpuObject->GetGpuObjectId();
        });
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
    std::lock_guard<std::mutex> lock1(gpuPassMutex);
    return gpuPassLoad.size();
}

size_t GpuResourceLoader::CountObjectsToUpdate()
{
    std::lock_guard<std::mutex> lock2(updateMutex);
    return objectsToUpdate.size();
}

size_t GpuResourceLoader::CountObjectsToRelease()
{
    std::lock_guard<std::mutex> lock3(releaseMutex);
    return objectsToRelease.size();
}

size_t GpuResourceLoader::CountOfProcessedTasks()
{
    std::scoped_lock lock(gpuPassMutex, updateMutex, releaseMutex, functionMutex);
    return objectsToRelease.size() + objectsToUpdate.size() + gpuPassLoad.size() + functions.size();
}
void GpuResourceLoader::clear()
{
    std::scoped_lock lock(gpuPassMutex, updateMutex, releaseMutex, functionMutex);
    functions.clear();
    gpuPassLoad.clear();
    objectsToRelease.clear();
    objectsToRelease.clear();
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
            LOG_DEBUG << "Is already loaded.";
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
            LOG_ERROR << "Object not loaded";
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
