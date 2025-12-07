#include "GpuResourceLoader.h"

#include <Logger/Log.h>
#include <Types.h>

#include <algorithm>

namespace GameEngine
{
namespace
{
const size_t MAX_OBJECTS_IN_QUEUE = 20000;

struct GpuAddFunctionWrapper : public IGpuObjectWrapper
{
    GpuAddFunctionWrapper(GpuObject& object)
        : object(&object)
    {
    }

    void execute() override
    {
        if (object)
            object->GpuLoadingPass();
    }

    void resetObject() override
    {
        object = nullptr;
    }

    GpuObject* object;
};

struct GpuUpdateFunctionWrapper : public IGpuObjectWrapper
{
    GpuUpdateFunctionWrapper(GpuObject& object)
        : object(&object)
    {
    }

    void execute() override
    {
        if (object)
        {
            object->UpdateGpuPass();
        }
    }

    void resetObject() override
    {
        object = nullptr;
    }

    GpuObject* object;
};

struct GpuReleaseFunctionWrapper : public IGpuObjectWrapper
{
    GpuReleaseFunctionWrapper(std::unique_ptr<GpuObject> object)
        : object(std::move(object))
    {
    }

    void execute() override
    {
        object->ReleaseGpuPass();
        object.reset();
    }

    void resetObject() override
    {
        object.reset();
    }

    std::unique_ptr<GpuObject> object;
};

struct GpuReleaseVectorFunctionWrapper : public IGpuObjectWrapper
{
    GpuReleaseVectorFunctionWrapper(std::vector<std::unique_ptr<GpuObject>>&& objects)
        : objects(std::move(objects))
    {
    }

    void execute() override
    {
        if (objects.empty())
            return;

        for (auto& object : objects)
        {
            object->ReleaseGpuPass();
        }

        resetObject();
    }

    void resetObject() override
    {
        objects.clear();
    }

    std::vector<std::unique_ptr<GpuObject>> objects;
};

struct GpuFunctionWrapper : public IGpuObjectWrapper
{
    GpuFunctionWrapper(std::function<void()> func)
        : function(std::move(func))
    {
    }

    void execute() override
    {
        if (function)
            function();
    }

    void resetObject() override
    {
        function = nullptr;
    }

    std::function<void()> function;
};
}  // namespace
GpuResourceLoader::GpuResourceLoader()
{
    objectsToExecute.reserve(MAX_OBJECTS_IN_QUEUE);
}

GpuResourceLoader::~GpuResourceLoader()
{
    LOG_DEBUG << "~GpuResourceLoader";
}
void GpuResourceLoader::AddFunctionToCall(std::function<void()> f)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    objectsToExecute.push_back(std::make_unique<GpuFunctionWrapper>(f));
}

void GpuResourceLoader::AddObjectToGpuLoadingPass(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    objectsToExecute.push_back(std::make_unique<GpuAddFunctionWrapper>(obj));
}

void GpuResourceLoader::AddObjectToUpdateGpuPass(GpuObject& obj)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    objectsToExecute.push_back(std::make_unique<GpuUpdateFunctionWrapper>(obj));
}

void GpuResourceLoader::AddObjectToRelease(std::unique_ptr<GpuObject> object)
{
    if (not object)
        return;

    std::lock_guard<std::mutex> lock(gpuPassMutex);
    objectsToExecute.push_back(std::make_unique<GpuReleaseFunctionWrapper>(std::move(object)));
}
void GpuResourceLoader::AddObjectsToRelease(std::vector<std::unique_ptr<GpuObject>>&& objects)
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    objectsToExecute.push_back(std::make_unique<GpuReleaseVectorFunctionWrapper>(std::move(objects)));
}

void GpuResourceLoader::RuntimeGpuTasks()
{
    std::vector<std::unique_ptr<IGpuObjectWrapper>> tmp;

    {
        std::lock_guard<std::mutex> lock(gpuPassMutex);
        tmp = std::move(objectsToExecute);
        objectsToExecute.reserve(MAX_OBJECTS_IN_QUEUE);
    }

    for (auto& object : tmp)
    {
        object->execute();
    }
}

size_t GpuResourceLoader::CountObjectsInQueues() const
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    return objectsToExecute.size();
}

void GpuResourceLoader::clear()
{
    std::lock_guard<std::mutex> lock(gpuPassMutex);
    objectsToExecute.clear();
}
}  // namespace GameEngine
