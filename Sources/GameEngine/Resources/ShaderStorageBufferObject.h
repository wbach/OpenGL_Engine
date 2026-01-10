#pragma once
#include "BufferObject.h"

namespace GameEngine
{
template <typename T>
class ShaderStorageBufferObject : public BaseBufferObject
{
public:
    ShaderStorageBufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
    {
    }

    ShaderStorageBufferObject(const T& data, GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
        , data_(data)
    {
    }
    ~ShaderStorageBufferObject()
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
        updatePoseBufferIdPool_.releaseId(id);
    }

    void UpdateGpuPass() override
    {
        if (graphicsObjectId_)
        {
            for (auto& [_, onUpdate] : onUpdateCallbacks_)
                onUpdate();

            graphicsApi_.UpdateShaderStorageBuffer(*graphicsObjectId_, &data_);
        }
    }

    void GpuLoadingPass() override
    {
        if (not graphicsObjectId_)
        {
            graphicsObjectId_ = graphicsApi_.CreateShaderStorageBuffer(bindLocation_, sizeof(T), GraphicsApi::DrawFlag::Dynamic);
        }

        if (graphicsObjectId_)
        {
            graphicsApi_.UpdateShaderStorageBuffer(*graphicsObjectId_, &data_);
        }
    }

protected:
    T data_;
    Utils::IdPool updatePoseBufferIdPool_;
    std::unordered_map<uint32, std::function<void()>> onUpdateCallbacks_;
};
}  // namespace GameEngine
