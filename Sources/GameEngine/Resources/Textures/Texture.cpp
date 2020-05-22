#include "Texture.h"

#include <Logger/Log.h>
#include <Utils.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>

#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
float GetTextureXOffset(uint32 textureIndex, uint32 numberOfRows)
{
    auto column = textureIndex % numberOfRows;
    return static_cast<float>(column) / static_cast<float>(numberOfRows);
}
float GetTextureYOffset(uint32 textureIndex, uint32 numberOfRows)
{
    auto row = textureIndex / numberOfRows;
    return static_cast<float>(row) / static_cast<float>(numberOfRows);
}
vec2 GetTextureOffset(uint32 textureIndex, uint32 numberOfRows)
{
    return vec2(GetTextureXOffset(textureIndex, numberOfRows), GetTextureYOffset(textureIndex, numberOfRows));
}

Texture::Texture(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , size_(0)
    , numberOfRows_(1)
    , orginalData_(true)
{
}

Texture::Texture(GraphicsApi::IGraphicsApi& graphicsApi, const GraphicsApi::ID& id)
    : graphicsApi_(graphicsApi)
    , size_(0)
    , numberOfRows_(1)
    , orginalData_(true)
{
    graphicsObjectId_ = id;
}

Texture::Texture(GraphicsApi::IGraphicsApi& graphicsApi, const vec2ui& size, const std::optional<File>& file)
    : graphicsApi_(graphicsApi)
    , file_(file)
    , size_(size)
    , numberOfRows_(1)
    , orginalData_(true)
{
    if (file_)
    {
        auto rows = GetNumberOfRowsBasedOnTextureFileName(file_->GetBaseName());
        if (rows)
        {
            numberOfRows_ = *rows;
        }
    }
}
Texture::~Texture()
{
    ReleaseGpuPass();
}
void Texture::GpuLoadingPass()
{
}
void Texture::ReleaseGpuPass()
{
    if (not graphicsObjectId_)
        return;

    auto filename = file_ ? file_->GetBaseName() : "";

    DEBUG_LOG("Clean gpu resources, " + filename + ", graphicsObjectId_=" + std::to_string(*graphicsObjectId_));

    graphicsApi_.DeleteObject(*graphicsObjectId_);
    GpuObject::ReleaseGpuPass();
}
std::optional<uint32> Texture::GetNumberOfRowsBasedOnTextureFileName(const std::string& file) const
{
    auto cfile = file;
    std::replace(cfile.begin(), cfile.end(), '\\', '/');
    auto v        = Utils::SplitString(cfile, '/');
    auto filename = v.back().substr(0, v.back().find_last_of('.'));

    auto rowsPos = filename.find("_rows_");

    if (rowsPos != std::string::npos)
    {
        auto rows = filename.substr(rowsPos + 6);
        if (not rows.empty())
        {
            return std::stoi(rows);
        }
    }

    return std::nullopt;
}
bool Texture::IsModified() const
{
    return not orginalData_;
}
void Texture::ResetModifyAcknowlage()
{
    orginalData_ = true;
}
uint32 Texture::getNumberOfRows() const
{
    return numberOfRows_;
}
}  // namespace GameEngine
