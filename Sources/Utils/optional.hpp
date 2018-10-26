#pragma once

namespace wb
{
template <class T>
class optional
{
public:
    optional() = default;
    optional(const T& v)
        : hasvalue_(true)
        , value_(v)
    {
    }

    optional(const optional<T>& v)
        : hasvalue_(v.hasvalue_)
        , value_(v.value_)
    {
    }

    bool operator==(const T& v) const
    {
        return value_ == v;
    }

    bool operator==(const optional<T>& v) const
    {
        if (hasvalue_ && v.hasvalue_)
            return value_ == v.value_;

        // both uninitialized
        if (!hasvalue_ && !v.hasvalue_)
            return true;

        return false;
    }

    bool operator!() const
    {
        return !hasvalue_;
    }
    operator bool() const
    {
        return hasvalue_;
    }

    bool operator>(const T& v) const
    {
        return value_ > v.value_;
    }

    bool operator<(const T& v) const
    {
        return value_ < v.value_;
    }
    bool operator>=(const T& v) const
    {
        return value_ >= v.value_;
    }
    bool operator<=(const T& v) const
    {
        return value_ <= v.value_;
    }
    optional operator=(const T& v)
    {
        this->value_    = v;
        this->hasvalue_ = true;
        return *this;
    }
    optional& operator++()
    {
        ++value_;
        return *this;
    }
    optional operator+(const T& v)
    {
        optional o;
        o.value_    = value_ + v;
        o.hasvalue_ = hasvalue_;
        return o;
    }
    optional operator-(const T& v)
    {
        optional o;
        o.value_    = value_ + v.value_;
        o.hasvalue_ = hasvalue_;
        return o;
    }
    optional operator*(const T& v)
    {
        optional o;
        o.value_    = value_ * v;
        o.hasvalue_ = hasvalue_;
        return o;
    }
    optional operator/(const T& v)
    {
        optional o;
        o.value_    = value_ / v;
        o.hasvalue_ = hasvalue_;
        return o;
    }
    const T& constValue() const
    {
        return value_;
    }
    T& value()
    {
        return value_;
    }

private:
    bool hasvalue_ = false;
    T value_;
};
}
