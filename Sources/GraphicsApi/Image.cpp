#include "Image.h"

#include <Logger/Log.h>

namespace GraphicsApi
{
struct ImageDataPtrVisitor
{
    const void* operator()(const std::vector<uint8>& input) const
    {
        return &input[0];
    }

    const void* operator()(const std::vector<float>& input) const
    {
        return &input[0];
    }
    template <class T>
    const void* operator()(const T&) const
    {
        return nullptr;
    }
};

struct ImageDataIsEmptyVisitor
{
    bool operator()(const std::vector<uint8>& input) const
    {
        return input.empty();
    }

    bool operator()(const std::vector<float>& input) const
    {
        return input.empty();
    }
    template <class T>
    bool operator()(const T&) const
    {
        return false;
    }
};

struct ImageDataClearVisitor
{
    void operator()(std::vector<uint8>& input) const
    {
        input.clear();
    }

    void operator()(std::vector<float>& input) const
    {
        input.clear();
    }
    template <class T>
    void operator()(const T&) const
    {
    }
};

class ImageDataSetVisitor
{
public:
    ImageDataSetVisitor(const vec2ui& position, const Color& color, uint32 width, uint8 channels)
        : color(color)
        , startIndex(channels * (position.x + position.y * width))
        , channels(channels)
    {
    }

    bool operator()(std::vector<uint8>& data) const
    {
        if (startIndex + (channels - 1) < data.size())
        {
            switch (channels)
            {
                case 1:
                    data[startIndex] = color.r();
                    break;
                case 2:
                    data[startIndex]     = color.r();
                    data[startIndex + 1] = color.g();
                    break;
                case 3:
                    data[startIndex]     = color.r();
                    data[startIndex + 1] = color.g();
                    data[startIndex + 2] = color.b();
                    break;
                case 4:
                    data[startIndex]     = color.r();
                    data[startIndex + 1] = color.g();
                    data[startIndex + 2] = color.b();
                    data[startIndex + 3] = color.a();
                    break;
            }
            return true;
        }
        return false;
    }

    bool operator()(std::vector<float>& data) const
    {
        if (startIndex + (channels - 1) < data.size())
        {
            switch (channels)
            {
                case 1:
                    data[startIndex] = color.value.x;
                    break;
                case 2:
                    data[startIndex]     = color.value.x;
                    data[startIndex + 1] = color.value.y;
                    break;
                case 3:
                    data[startIndex]     = color.value.x;
                    data[startIndex + 1] = color.value.y;
                    data[startIndex + 2] = color.value.z;
                    break;
                case 4:
                    data[startIndex]     = color.value.x;
                    data[startIndex + 1] = color.value.y;
                    data[startIndex + 2] = color.value.z;
                    data[startIndex + 3] = color.value.w;
                    break;
            }
            return true;
        }
        return false;
    }

    template <class T>
    bool operator()(const T&) const
    {
        return false;
    }

private:
    const Color& color;
    uint32 startIndex;
    uint8 channels;
};

class ImageDataGetVisitor
{
public:
    ImageDataGetVisitor(const vec2ui& position, uint32 width, uint8 channels)
        : startIndex(channels * (position.x + position.y * width))
        , channels(channels)
    {
    }

    std::optional<Color> operator()(const std::vector<uint8>& data) const
    {
        if (startIndex + (channels - 1) < data.size())
        {
            switch (channels)
            {
                case 1:
                    return Color(data[startIndex], 0, 0, 0);
                case 2:
                    return Color(data[startIndex], data[startIndex + 1], 0, 0);
                case 3:
                    return Color(data[startIndex], data[startIndex + 1], data[startIndex + 2], 0);
                case 4:
                    return Color(data[startIndex], data[startIndex + 1], data[startIndex + 2], data[startIndex + 3]);
            }
        }
        return std::nullopt;
    }

    std::optional<Color> operator()(const std::vector<float>& data) const
    {
        if (startIndex + (channels - 1) < data.size())
        {
            switch (channels)
            {
                case 1:
                    return Color(data[startIndex], 0.f, 0.f, 0.f);
                case 2:
                    return Color(data[startIndex], data[startIndex + 1], 0.f, 0.f);
                case 3:
                    return Color(data[startIndex], data[startIndex + 1], data[startIndex + 2], 0.f);
                case 4:
                    return Color(data[startIndex], data[startIndex + 1], data[startIndex + 2], data[startIndex + 3]);
            }
        }
        return std::nullopt;
    }

    template <class T>
    std::optional<Color> operator()(const T&) const
    {
        return std::nullopt;
    }

private:
    uint32 startIndex;
    uint8 channels;
};

bool Image::setPixel(const vec2ui& position, const Color& color)
{
    return std::visit(ImageDataSetVisitor(position, color, width, channels_), data_);
}
Image::~Image()
{
}

vec2ui Image::size() const
{
    return vec2ui(width, height);
}

const void* GraphicsApi::Image::getRawDataPtr() const
{
    return std::visit(ImageDataPtrVisitor(), data_);
}

std::optional<Color> Image::getPixel(const vec2ui& position) const
{
    return std::visit(ImageDataGetVisitor(position, width, channels_), data_);
}
bool Image::empty() const
{
    return data_.index() == 0 or std::visit(ImageDataIsEmptyVisitor(), data_);
}
void Image::clearData()
{
    std::visit(ImageDataClearVisitor(), data_);
}
void Image::setChannels(uint8 channels)
{
    if (channels > 0 and channels <= 4)
    {
        channels_ = channels;
    }
    else
    {
        ERROR_LOG("Unsupported channels count :" + std::to_string(channels) +
                  ", current value : " + std::to_string(channels_));
    }
}
uint8 Image::getChannelsCount() const
{
    return channels_;
}
const ImageData& Image::getImageData() const
{
    return data_;
}
}  // namespace GraphicsApi
