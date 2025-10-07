#include "BoundingBox.h"

#include <ostream>

namespace GameEngine
{
BoundingBox::BoundingBox(NumericLimits)
    : min_(std::numeric_limits<float>::max())
    , max_(-std::numeric_limits<float>::max())
{
    calculate();
}

BoundingBox::BoundingBox(const vec3 &min, const vec3 &max)
    : min_(min)
    , max_(max)
{
    calculate();
}

void BoundingBox::minMax(const vec3 &min, const vec3 &max)
{
    min_ = min;
    max_ = max;
    calculate();
}

void BoundingBox::minY(float v)
{
    min_.y = v;
    calculate();
}

void BoundingBox::maxY(float v)
{
    max_.y = v;
    calculate();
}

void BoundingBox::min(const vec3 &min)
{
    min_ = min;
    calculate();
}

void BoundingBox::max(const vec3 &max)
{
    max_ = max;
    calculate();
}

void BoundingBox::scale(const vec3 &scaleFactor)
{
    min_ *= scaleFactor;
    max_ *= scaleFactor;
    calculate();
}

void BoundingBox::translate(const vec3 &translation)
{
    min_ += translation;
    max_ += translation;
    calculate();
}

const vec3 &BoundingBox::min() const
{
    return min_;
}

const vec3 &BoundingBox::max() const
{
    return max_;
}

const vec3 &BoundingBox::center() const
{
    return center_;
}

const vec3 &BoundingBox::size() const
{
    return size_;
}

float BoundingBox::maxScale() const
{
    return maxScale_;
}

void BoundingBox::calculate()
{
    size_     = glm::abs(max_ - min_);
    center_   = (min_ + max_) * .5f;
    maxScale_ = glm::compMax(size_);
}

BoundingBox BoundingBox::transformed(const glm::mat4& transform) const
{
    std::array<glm::vec3, 8> corners = {
        glm::vec3(min_.x, min_.y, min_.z),
        glm::vec3(max_.x, min_.y, min_.z),
        glm::vec3(min_.x, max_.y, min_.z),
        glm::vec3(max_.x, max_.y, min_.z),
        glm::vec3(min_.x, min_.y, max_.z),
        glm::vec3(max_.x, min_.y, max_.z),
        glm::vec3(min_.x, max_.y, max_.z),
        glm::vec3(max_.x, max_.y, max_.z)
    };

    glm::vec3 newMin(std::numeric_limits<float>::max());
    glm::vec3 newMax(-std::numeric_limits<float>::max());

    for (const auto& c : corners)
    {
        glm::vec4 transformed = transform * glm::vec4(c, 1.0f);
        glm::vec3 p = glm::vec3(transformed);
        newMin = glm::min(newMin, p);
        newMax = glm::max(newMax, p);
    }

    return BoundingBox(newMin, newMax);
}

void BoundingBox::expandToInclude(const BoundingBox& other)
{
    min_.x = std::min(min_.x, other.min_.x);
    min_.y = std::min(min_.y, other.min_.y);
    min_.z = std::min(min_.z, other.min_.z);

    max_.x = std::max(max_.x, other.max_.x);
    max_.y = std::max(max_.y, other.max_.y);
    max_.z = std::max(max_.z, other.max_.z);

    calculate();
}

std::ostream &operator<<(std::ostream &os, const BoundingBox &box)
{
    os << "BoundingBox(min: (" << box.min().x << ", " << box.min().y << ", " << box.min().z << "), "
       << "max: (" << box.max().x << ", " << box.max().y << ", " << box.max().z << "), "
       << "center: (" << box.center().x << ", " << box.center().y << ", " << box.center().z << "), "
       << "size: (" << box.size().x << ", " << box.size().y << ", " << box.size().z << "), "
       << "maxScale: " << box.maxScale() << ")";
    return os;
}
}  // namespace GameEngine
