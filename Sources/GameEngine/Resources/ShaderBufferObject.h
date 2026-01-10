#pragma once
#include "BufferObject.h"

namespace GameEngine
{
template <typename T>
class ShaderBufferObject : public BaseBufferObject
{
public:
    ShaderBufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
    {
    }

    ShaderBufferObject(const T& data, GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
        , data_(data)
    {
    }
    ~ShaderBufferObject()
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

            graphicsApi_.UpdateShaderBuffer(*graphicsObjectId_, &data_);
        }
    }

    void GpuLoadingPass() override
    {
        if (not graphicsObjectId_)
        {
            graphicsObjectId_ = graphicsApi_.CreateShaderBuffer(bindLocation_, sizeof(T), GraphicsApi::DrawFlag::Dynamic);
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
