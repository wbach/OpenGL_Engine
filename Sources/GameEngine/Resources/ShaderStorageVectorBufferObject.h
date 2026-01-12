#pragma once
#include <cstddef>

#include "BufferObject.h"

namespace GameEngine
{
template <typename T>
class ShaderStorageVectorBufferObject : public BaseBufferObject
{
public:
    ShaderStorageVectorBufferObject(GraphicsApi::IGraphicsApi& graphicsApi, uint32 bindLocation)
        : BaseBufferObject(graphicsApi, bindLocation)
    {
    }

    uint32 PushBack(const T& element)
    {
        dataVector_.push_back(element);
        return static_cast<uint32>(dataVector_.size() - 1);
    }

    void Clear()
    {
        dataVector_.clear();
    }

    std::vector<T>& GetData()
    {
        return dataVector_;
    }

    void GpuLoadingPass() override
    {
        if (dataVector_.empty())
            return;

        auto totalSize = getTotalSize();
        if (not graphicsObjectId_)
        {
            graphicsObjectId_ = graphicsApi_.CreateShaderStorageBuffer(bindLocation_, totalSize, GraphicsApi::DrawFlag::Dynamic);
        }

        graphicsApi_.UpdateShaderStorageBuffer(*graphicsObjectId_, dataVector_.data(), totalSize);
    }

    void UpdateGpuPass() override
    {
        GpuLoadingPass();
    }

    size_t GetCount() const
    {
        return dataVector_.size();
    }

private:
    uint32 getTotalSize() const
    {
        return static_cast<uint32>(dataVector_.size() * sizeof(T));
    }

protected:
    std::vector<T> dataVector_;
};
}  // namespace GameEngine
