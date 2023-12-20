#pragma once
#include <vector>

#include "GpuObject.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Types.h"
#include <Utils/IdPool.h>

namespace GameEngine
{
class BaseBufferObject : public GpuObject
{
public:
    BaseBufferObject(GraphicsApi::IGraphicsApi&, uint32);
    ~BaseBufferObject() override;
    void ReleaseGpuPass() override;
    void UpdateGpuPass() override;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    uint32 bindLocation_;
};

template <typename T>
class BufferObject : public BaseBufferObject
{
public:
    BufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
    {
    }

    BufferObject(const T& data, GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
        , data_(data)
        , oncall_{nullptr}
    {
    }
    ~BufferObject()
    {
    }

    T& GetData()
    {
        return data_;
    }

    uint32 SubscribeForUpdate(std::function<void()> onUpdate)
    {
        auto id = updatePoseBufferIdPool_.getId();
        onUpdateCallbacks_.insert({id, onUpdate});
        return id;
    }

    void UnSubscribeForUpdate(uint32 id)
    {
        onUpdateCallbacks_.erase(id);
    }

    void UpdateGpuPass() override
    {
        if (graphicsObjectId_)
        {
            for (auto& [_, onUpdate] : onUpdateCallbacks_)
                onUpdate();

            graphicsApi_.UpdateShaderBuffer(*graphicsObjectId_, &data_);
        }
    }

    void GpuLoadingPass() override
    {
        if (not graphicsObjectId_)
        {
            graphicsObjectId_ = graphicsApi_.CreateShaderBuffer(bindLocation_, sizeof(T));
        }

        if (graphicsObjectId_)
        {
            graphicsApi_.UpdateShaderBuffer(*graphicsObjectId_, &data_);
        }
    }

protected:
    T data_;
    Utils::IdPool updatePoseBufferIdPool_;
    std::unordered_map<uint32, std::function<void()>> onUpdateCallbacks_;
};

}  // namespace GameEngine
