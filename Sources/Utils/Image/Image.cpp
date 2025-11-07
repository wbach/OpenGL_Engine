#include "Image.h"

#include <Logger/Log.h>

namespace Utils
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

Image::Image(Image&& other) noexcept
    : width(other.width)
    , height(other.height)
    , channels_(other.channels_)
    , data_(std::move(other.data_))
{
    other.width     = 0;
    other.height    = 0;
    other.channels_ = 4;
}

Image& Image::operator=(Image&& other) noexcept
{
    if (this != &other)
    {
        width     = other.width;
        height    = other.height;
        channels_ = other.channels_;
        data_     = std::move(other.data_);

        // reset źródła
        other.width     = 0;
        other.height    = 0;
        other.channels_ = 4;
    }
    return *this;
}

Image::~Image()
{
}

vec2ui Image::size() const
{
    return vec2ui(width, height);
}

const void* Image::getRawDataPtr() const
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
        LOG_WARN << "Unsupported channels count :" << channels << ", current value : " << channels_;
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

ImageData& Image::getImageData()
{
    return data_;
}

void Image::applyFilter(const ImageFilter& imageFilter)
{
    auto& size = imageFilter.size;

    if (size.x % 2 == 0 or size.y % 2 == 0)
    {
        LOG_ERROR << "Wrong filter size. Filter not applied";
        return;
    }
    LOG_DEBUG << "Applying filter";
    uint32 halfsizeY = size.y / 2;
    uint32 halfsizeX = size.x / 2;

    for (uint32 y = halfsizeY; y < height - halfsizeY; ++y)
    {
        for (uint32 x = halfsizeX; x < width - halfsizeX; ++x)
        {
            Color pixelColor(0.f);

            for (uint32 fy = 0; fy < size.y; ++fy)
            {
                for (uint32 fx = 0; fx < size.x; ++fx)
                {
                    uint32 imageX = (x + fx + width - halfsizeX) % width;
                    uint32 imageY = (y + fy + height - halfsizeY) % height;

                    auto color = getPixel(vec2ui(imageX, imageY));
                    if (color)
                    {
                        auto maybeFilterWight = imageFilter.getSample(fx, fy);
                        if (maybeFilterWight)
                        {
                            *color *= *maybeFilterWight;
                            pixelColor += *color;
                        }
                    }
                }
            }
            setPixel(vec2ui(x, y), pixelColor);
        }
    }
}
}  // namespace Utils
