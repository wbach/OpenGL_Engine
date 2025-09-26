#include "GpuResourceLoader.h"

#include <Logger/Log.h>
#include <Types.h>

#include <algorithm>

namespace GameEngine
{
GpuResourceLoader::GpuResourceLoader()
{
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

    auto f = std::move(functions.front());
    functions.pop_front();
    f();
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

    GpuObject* obj = gpuPassLoad.front();
    gpuPassLoad.pop_front();
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
    if (objectsToUpdate.empty())
        return nullptr;

    GpuObject* obj = objectsToUpdate.front();
    objectsToUpdate.pop_front();
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
    if (objectsToRelease.empty())
        return nullptr;

    auto object = std::move(objectsToRelease.front());
    objectsToRelease.pop_front();
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
        auto iter =
            std::find_if(gpuPassLoad.begin(), gpuPassLoad.end(),
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
    std::lock_guard<std::mutex> lock(gpuPassMutex);

    GpuObject* obj{nullptr};
    do
    {
        obj = GetObjectToGpuLoadingPass();

        if (obj and not obj->GetGraphicsObjectId())
        {
            obj->GpuLoadingPass();
        }

    } while (obj);
}

void GpuResourceLoader::RuntimeUpdateObjectGpu()
{
    std::lock_guard<std::mutex> lock(updateMutex);
    GpuObject* obj{nullptr};
    do
    {
        obj = GetObjectToUpdateGpuPass();

        if (obj and obj->GetGraphicsObjectId())
        {
            obj->UpdateGpuPass();
        }

    } while (obj);
}

void GpuResourceLoader::RuntimeReleaseObjectGpu()
{
    std::lock_guard<std::mutex> lock(releaseMutex);
    std::unique_ptr<GpuObject> obj{nullptr};
    do
    {
        obj = GetObjectToRelease();

        if (obj and obj->GetGraphicsObjectId())
        {
            obj->ReleaseGpuPass();
            obj.reset();
        }

    } while (obj);
}
}  // namespace GameEngine
