#pragma once
#include <optional>
#include <variant>
#include <vector>

#include "Types.h"

namespace GraphicsApi
{
typedef std::variant<std::monostate, std::vector<uint8>, std::vector<float>> ImageData;

class Image
{
public:
    Image() = default;
    ~Image();
    uint32 width{0};
    uint32 height{0};

    vec2ui size() const;
    const void* getRawDataPtr() const;
    std::optional<Color> getPixel(const vec2ui&) const;
    bool setPixel(const vec2ui&, const Color&);
    bool empty() const;
    void clearData();
    void setChannels(uint8);
    uint8 getChannelsCount() const;
    const ImageData& getImageData() const;
    void applyFilter(const std::vector<float>&, const vec2ui&);

    template <typename T>
    void allocateImage()
    {
        std::vector<T> input;
        input.resize(channels_ * width * height);
        memset(&input[0], 0, input.size());
        data_ = std::move(input);
    }
    template <typename T>
    void copyImage(void* rawData)
    {
        T* buff = reinterpret_cast<T*>(rawData);
        auto len = channels_ * width * height;
        data_ = std::vector<T>(buff, buff + len);
    }
    template <typename T>
    void moveData(std::vector<T> input)
    {
        data_ = std::move(input);
    }

private:
    uint8 channels_{4};
    ImageData data_;
};
}  // namespace GraphicsApi
