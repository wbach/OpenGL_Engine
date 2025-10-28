#pragma once
#include <mutex>
#include <optional>
#include <variant>
#include <vector>

#include "ImageFilters.h"
#include "Types.h"

namespace Utils
{
typedef std::variant<std::monostate, std::vector<uint8>, std::vector<float>> ImageData;

class Image
{
public:
    Image() = default;
    Image(Image&& other) noexcept;

    ~Image();

    Image& operator=(Image&& other) noexcept;

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
    void applyFilter(const ImageFilter&);

    template <typename T>
    void allocateImage()
    {
        std::vector<T> input;
        input.resize(channels_ * width * height);
        memset(&input[0], 0, input.size());
        data_ = std::move(input);
    }
    template <typename T>
    void copyImage(const void* rawData)
    {
        auto buff = reinterpret_cast<const T*>(rawData);
        auto len  = channels_ * width * height;
        data_     = std::vector<T>(buff, buff + len);
    }
    template <typename T>
    void moveData(std::vector<T>&& input)
    {
        data_ = std::move(input);
    }

private:
    uint8 channels_{4};
    ImageData data_;
    mutable std::mutex mutex;
};
}  // namespace Utils
