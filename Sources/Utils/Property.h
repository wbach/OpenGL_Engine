#pragma once
#include <ostream>

template <typename T>
class Property
{
public:
    Property(T initValue, bool* dirty)
        : value(initValue)
        , dirtyFlag(dirty)
    {
        if (not dirty)
        {
            dirtyFlag = &ownDirtyFlag;
        }
    }

    Property(T initValue)
        : value(initValue)
        , dirtyFlag(&ownDirtyFlag)
    {
    }

    Property()
        : value()
        , dirtyFlag(&ownDirtyFlag)
    {
    }

    Property(const Property&) = delete;

    Property& operator=(const T& newValue)
    {
        if constexpr (requires { value != newValue; })
        {
            if (value == newValue)
                return *this;
        }

        value = newValue;
        if (dirtyFlag)
            *dirtyFlag = true;
        return *this;
    }

    auto operator<=>(const Property& other) const
    {
        return value <=> other.value;
    }
    bool operator==(const Property& other) const
    {
        return value == other.value;
    }

    auto operator<=>(const T& other) const
    {
        return value <=> other;
    }

    bool operator==(const T& other) const
    {
        return value == other;
    }

    const T& get() const
    {
        return value;
    }
    operator const T&() const
    {
        return value;
    }
    const T* operator->() const
    {
        return &value;
    }
    const T& operator*() const
    {
        return value;
    }

    T& modify()
    {
        if (dirtyFlag)
            *dirtyFlag = true;
        return value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Property& prop)
    {
        if constexpr (requires { os << *prop.value; })
        {
            if constexpr (requires { prop.value.has_value(); })
            {
                if (prop.value.has_value())
                {
                    os << *prop.value;
                }
                else
                {
                    os << "std::nullopt";
                }
            }
            else
            {
                os << prop.value;
            }
        }
        else
        {
            os << "[complex type]";
        }
        return os;
    }

    bool* GetDirtyPtr()
    {
        return dirtyFlag;
    }

    bool isDirty() const
    {
        return dirtyFlag and *dirtyFlag;
    }
    void clearDirty()
    {
        *dirtyFlag = false;
    }

private:
    T value;
    bool* dirtyFlag;
    bool ownDirtyFlag{false};
};
