#include "BoundingBox.h"

#include <ostream>

namespace GameEngine
{
BoundingBox::BoundingBox(NumericLimits)
    : min_(std::numeric_limits<float>::max())
    , max_(-std::numeric_limits<float>::max())
{
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
    center_   = (min_ + min_) * .5f;
    maxScale_ = glm::compMax(size_);
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
