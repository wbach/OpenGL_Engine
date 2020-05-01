#include "HeightMap.h"
#include <Logger/Log.h>
#include <algorithm>
#include "GameEngine/Renderers/Objects/Terrain/TerrainNormalMapRenderer.h"

namespace GameEngine
{
HeightMap::HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, bool keepData, const std::string& file,
                     const std::string& filepath, Image image)
    : Texture(graphicsApi, file, filepath)
    , image_(std::move(image))
    , keepData_(keepData)
    , maximumHeight_(0)
{
    size_          = vec2ui(image_.width, image_.height);
    CalculateMaximumHeight();
}

void HeightMap::GpuLoadingPass()
{
    if (image_.floatData.empty() or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture : " + filename + ". floatData is null or is initialized.");
        return;
    }

    DEBUG_LOG("Create texutre filneame : " + fullpath);
    auto graphicsObjectId = graphicsApi_.CreateTexture(
        GraphicsApi::TextureType::FLOAT_TEXTURE_1C, GraphicsApi::TextureFilter::LINEAR,
        GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE, size_, &image_.floatData[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("File " + filename + " is in GPU.");
    }
    else
    {
        ERROR_LOG("Texutre not created. Filename : " + fullpath);
    }

    if (not keepData_)
    {
        image_.floatData.clear();
    }
}

const Image& HeightMap::GetImage() const
{
    return image_;
}

Image& HeightMap::GetImage()
{
    return image_;
}

void HeightMap::SetScale(const vec3& scale)
{
    scale_ = scale;
}

const vec3& HeightMap::GetScale() const
{
    return scale_;
}

float HeightMap::GetMaximumHeight() const
{
    return maximumHeight_;
}

bool HeightMap::SetHeight(const vec2ui& cooridnate, float value)
{
    if (cooridnate.x < image_.width and cooridnate.y < image_.height)
    {
        auto index       = cooridnate.x + cooridnate.y * image_.width;
        auto actualValue = image_.floatData[index];

        if (not compare(actualValue, value))
        {
            image_.floatData[index] = value;
          //  if (value > maximumHeight_)
             //   maximumHeight_ = value;
            return true;
        }
        else
        {
            DEBUG_LOG("Setting value is almost the same. (Set/Actual)" + std::to_string(value) + "|" + std::to_string(actualValue ));
        }
    }
    return false;
}
std::optional<float> HeightMap::GetHeight(const vec2ui& cooridnate)
{
    if (cooridnate.x < image_.width and cooridnate.y < image_.height)
    {
        auto index = cooridnate.x + cooridnate.y * image_.width;
        return image_.floatData[index];
    }

    return std::nullopt;
}
void HeightMap::CalculateMaximumHeight()
{
    maximumHeight_ = *std::max_element(image_.floatData.begin(), image_.floatData.end());
}
}  // namespace GameEngine
