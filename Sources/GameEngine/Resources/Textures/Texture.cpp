#include "Texture.h"

#include <algorithm>

#include "Logger/Log.h"
#include "Utils.h"

namespace GameEngine
{
float GetTextureXOffset(uint32 textureIndex, uint32 numberOfRows)
{
    int column = textureIndex % numberOfRows;
    return static_cast<float>(column) / static_cast<float>(numberOfRows);
}
float GetTextureYOffset(uint32 textureIndex, uint32 numberOfRows)
{
    int row = textureIndex / numberOfRows;
    return static_cast<float>(row) / static_cast<float>(numberOfRows);
}
vec2 GetTextureOffset(uint32 textureIndex, uint32 numberOfRows)
{
    return vec2(GetTextureXOffset(textureIndex, numberOfRows), GetTextureYOffset(textureIndex, numberOfRows));
}

Texture::Texture(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
    , size_(0)
{
}

Texture::Texture(GraphicsApi::IGraphicsApi& graphicsApi, const GraphicsApi::ID& id)
    : graphicsApi_(graphicsApi)
    , size_(0)
{
    graphicsObjectId_ = id;
}

Texture::Texture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& file, const std::string& filepath, const vec2ui &size,
                 bool applySizeLimit)
    : graphicsApi_(graphicsApi)
    , filename(file)
    , fullpath(filepath)
    , size_(size)
    , applySizeLimit(applySizeLimit)
{
    auto rows = GetNumberOfRowsBasedOnTextureFileName(file);
    if (rows)
    {
        numberOfRows = *rows;
    }
    else
    {
        numberOfRows = 1;
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
}  // namespace GameEngine
