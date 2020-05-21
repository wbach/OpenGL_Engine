#include "HeightMap.h"

#include <Logger/Log.h>

#include <algorithm>

namespace GameEngine
{
HeightMap::HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, const File& file, Image image)
    : Texture(graphicsApi, file, vec2ui(image.width, image.height))
    , image_(std::move(image))
    , maximumHeight_(0)
{
    UpdateMaximumHeight();
}

void HeightMap::GpuLoadingPass()
{
    if (image_.floatData.empty() or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture : " + file_->GetBaseName() +
                  ". floatData is null or is initialized.");
        return;
    }
    DEBUG_LOG("Create texutre filneame : " + file_->GetBaseName());

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(GraphicsApi::TextureType::FLOAT_TEXTURE_1C, GraphicsApi::TextureFilter::LINEAR,
                                   GraphicsApi::TextureMipmap::NONE, size_, &image_.floatData[0]);

    if (graphicsObjectId)
    {
        graphicsObjectId_ = *graphicsObjectId;
        DEBUG_LOG("File " + file_->GetBaseName() + " is in GPU.");
    }
    else
    {
        ERROR_LOG("Texutre not created. Filename : " + file_->GetBaseName());
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

float HeightMap::GetMinimumHeight() const
{
    return minimumHeight_;
}

float HeightMap::GetDeltaHeight() const
{
    return deltaHeight_;
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
            orginalData_            = false;
            return true;
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
bool HeightMap::UpdateMaximumHeight()
{
    auto maximumHeight = *std::max_element(image_.floatData.begin(), image_.floatData.end());
    auto minimumHeight = *std::min_element(image_.floatData.begin(), image_.floatData.end());

    if (not compare(maximumHeight_, maximumHeight) or not compare(minimumHeight_, minimumHeight))
    {
        minimumHeight_ = minimumHeight;
        maximumHeight_ = maximumHeight;
        deltaHeight_   = maximumHeight_ - minimumHeight;
        return true;
    }

    return false;
}
}  // namespace GameEngine
