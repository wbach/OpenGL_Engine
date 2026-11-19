#pragma once
#include <Logger/Log.h>
#include <Utils.h>

#include <optional>
#include <variant>
#include <vector>

#include "ImageFilters.h"
#include "Types.h"

namespace Utils
{
template <typename IntT>
constexpr float maxValueAsFloat() noexcept
{
    static_assert(std::is_integral_v<IntT>, "IntT must be an integral type");
    return static_cast<float>(std::numeric_limits<IntT>::max());
}

typedef std::variant<std::monostate, std::vector<uint8>, std::vector<uint16>, std::vector<float>> ImageData;

class ENGINE_API Image
{
public:
    Image() = default;
    Image(Image&&) noexcept;

    ~Image();

    Image& operator=(const Image& other);
    Image& operator=(Image&& other) noexcept;
    Image(const Image& other);

    vec2ui size() const;
    const void* getRawDataPtr() const;
    std::optional<Color> getPixel(const vec2ui&) const;
    bool setPixel(const vec2ui&, const Color&);
    bool empty() const;
    void clearData();
    void setChannels(uint8);
    uint8 getChannelsCount() const;
    const ImageData& getImageData() const;
    ImageData& getImageData();
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
    void moveData(ImageData&& input)
    {
        data_ = std::move(input);
    }

    template <typename TargetT>
    bool convertDataType()
    {
        static_assert(std::is_same_v<TargetT, uint8> || std::is_same_v<TargetT, uint16> || std::is_same_v<TargetT, float>,
                      "TargetT must be uint8, uint16, or float");

        return std::visit(
            [this](auto& currentVec) -> bool
            {
                using CurrentVecT = std::decay_t<decltype(currentVec)>;

                if constexpr (std::is_same_v<CurrentVecT, std::monostate>)
                {
                    return false;
                }
                else
                {
                    using SourceT = typename CurrentVecT::value_type;

                    if constexpr (std::is_same_v<SourceT, TargetT>)
                    {
                        return true;
                    }
                    else
                    {
                        std::vector<TargetT> converted;
                        converted.reserve(currentVec.size());

                        for (const SourceT val : currentVec)
                        {
                            const float norm = toNormFloat(val);
                            converted.push_back(fromNormFloat<TargetT>(norm));
                        }

                        data_ = std::move(converted);
                        LOG_DEBUG << "Image data converted. From " << Utils::GetTypeName<SourceT>() << " to "
                                  << Utils::GetTypeName<TargetT>();
                        return true;
                    }
                }
            },
            data_);
    }

    void compressData();
    void decompressData();

    void resizeImage(uint32, uint32);
    void resizeImage(const vec2ui&);
    void enforceMaxDimensions(const vec2ui&);

    uint32 width{0};
    uint32 height{0};
    uint8 channels_{4};
    ImageData data_;

    bool isCompressed{false};
    std::optional<size_t> pitch;

private:
    template <typename SourceT>
    static float toNormFloat(SourceT value) noexcept
    {
        if constexpr (std::is_same_v<SourceT, float>)
        {
            return value;
        }
        else
        {
            return static_cast<float>(value) / maxValueAsFloat<SourceT>();
        }
    }

    template <typename TargetT>
    static TargetT fromNormFloat(float norm) noexcept
    {
        if constexpr (std::is_same_v<TargetT, float>)
        {
            return norm;
        }
        else
        {
            const float clamped = std::clamp(norm, 0.f, 1.f);
            return static_cast<TargetT>(std::lround(clamped * maxValueAsFloat<TargetT>()));
        }
    }
};

std::ostream& operator<<(std::ostream&, const Utils::Image&);
}  // namespace Utils
