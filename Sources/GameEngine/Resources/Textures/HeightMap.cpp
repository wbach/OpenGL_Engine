#include "HeightMap.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>

#include <algorithm>

namespace GameEngine
{
HeightMap::HeightMap(GraphicsApi::IGraphicsApi& graphicsApi, const TextureParameters& params, const File& file, GraphicsApi::Image image)
    : Texture(graphicsApi, params, vec2ui(image.width, image.height), file)
    , image_(std::move(image))
    , maximumHeight_(0)
{
    if (image_.getChannelsCount() > 1)
    {
        ERROR_LOG("Height map shouldbe one channel float data, but is : " + std::to_string(image.getChannelsCount()));
    }

    UpdateMaximumHeight();
}

void HeightMap::GpuLoadingPass()
{
    if (image_.empty() or graphicsObjectId_)
    {
        ERROR_LOG("There was an error loading the texture : " + file_->GetBaseName() +
                  ". floatData is null or is initialized.");
        return;
    }
    DEBUG_LOG("Create texutre filneame : " + file_->GetBaseName());

    auto graphicsObjectId =
        graphicsApi_.CreateTexture(image_, GraphicsApi::TextureFilter::LINEAR, GraphicsApi::TextureMipmap::NONE);

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
const GraphicsApi::Image& HeightMap::GetImage() const
{
    return image_;
}
void HeightMap::setImage(GraphicsApi::Image image)
{
    image_ = std::move(image);
    size_ = vec2ui(image.width, image.height);
    orginalData_ = false;
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
    if (image_.getChannelsCount() > 1)
    {
        ERROR_LOG("Only one channel in height map is supported");
        return false;
    }

    auto currentValue = image_.getPixel(cooridnate);

    if (currentValue)
    {
        auto actualValue = currentValue->value.x;

        if (not compare(actualValue, value))
        {
            image_.setPixel(cooridnate, Color(value));
            orginalData_ = false;
            return true;
        }
    }
    return false;
}
std::optional<float> HeightMap::GetHeight(const vec2ui& cooridnate)
{
    auto currentValue = image_.getPixel(cooridnate);

    if (currentValue)
        return currentValue->value.x;

    return std::nullopt;
}
bool HeightMap::UpdateMaximumHeight()
{
    auto maximumHeight{0.f};
    auto minimumHeight{0.f};

    std::visit(visitor{[&](const std::vector<uint8>& data) {
                           maximumHeight = static_cast<float>(*std::max_element(data.begin(), data.end()) / 255.f);
                           minimumHeight = static_cast<float>(*std::min_element(data.begin(), data.end()) / 255.f);
                       },
                       [&](const std::vector<float>& data) {
                           maximumHeight = *std::max_element(data.begin(), data.end());
                           minimumHeight = *std::min_element(data.begin(), data.end());
                       },
                       [](std::monostate) { ERROR_LOG("Image data not set!."); }},
               image_.getImageData());

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
