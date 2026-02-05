#pragma once
#include <Logger/Log.h>
#include <Types.h>
#include <Utils/IdPool.h>

#include <Mutex.hpp>
#include <algorithm>
#include <functional>
#include <memory>

#include "IConfigurationParam.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Params
{
template <class T>
class ConfigurationEnumParam : public IConfigurationParam
{
public:
    ConfigurationEnumParam(const T& v)
        : value_{v}
        , defaultValueIndex_{0}
        , defaultValues_{magic_enum::enum_values<T>()}
        , isLocked_{false}
    {
        updatetValueIndex();
    }
    const std::vector<T>& GetDefaultValues() const
    {
        return defaultValues_;
    }
    std::vector<std::string> getValuesAsStrings() const override
    {
        std::vector<std::string> result;
        for (const auto& value : defaultValues_)
        {
            result.push_back(std::string(magic_enum::enum_name(value)));
        }
        return result;
    }
    size_t getValueIndex() const override
    {
        return defaultValueIndex_;
    }

    std::string toString() const override
    {
        return std::string(magic_enum::enum_name(value_));
    }

    void fromString(const std::string& value)
    {
        if (value.empty())
        {
            LOG_DEBUG << "Input is empty";
            return;
        }

        if (auto maybeValue = magic_enum::enum_cast<T>(value))
        {
            //set(*maybeValue);
            value_ = *maybeValue;
        }
        else
        {
            LOG_WARN << "Enum cast error : " << value;
        }
    }
    std::string next() override
    {
        if (isLocked_)
            return toString();

        if (not defaultValues_.empty())
        {
            ++defaultValueIndex_;

            if (defaultValueIndex_ > defaultValues_.size() - 1)
            {
                defaultValueIndex_ = 0;
            }

            return std::string(magic_enum::enum_name(defaultValues_[defaultValueIndex_]));
        }

        return toString();
    }
    std::string previous() override
    {
        if (isLocked_)
            return toString();

        if (not defaultValues_.empty())
        {
            if (defaultValueIndex_ == 0)
            {
                defaultValueIndex_ = defaultValues_.size() - 1;
            }
            else
            {
                --defaultValueIndex_;
            }

            return std::string(magic_enum::enum_name(defaultValues_[defaultValueIndex_]));
        }
        return toString();
    }
    void apply() override
    {
        if (isLocked_)
            return;

        if (not defaultValues_.empty())
        {
            if (defaultValueIndex_ > defaultValues_.size())
            {
                LOG_ERROR << "Param index out of range";
                return;
            }
            set(defaultValues_[defaultValueIndex_]);
        }
    }
    const T& get() const
    {
        return value_;
    }
    void set(const T& value)
    {
        LOG_DEBUG << magic_enum::enum_name(value);
        if (isLocked_)
            return;

        if (value_ == value)
            return;

        std::lock_guard<std::mutex> lk(subscriberMock_);
        value_ = value;

        for (const auto& subscriber : subscribers_)
        {
            subscriber.second();
        }
        updatetValueIndex();
    }
    void setValueFromIndex(size_t i) override
    {
        defaultValueIndex_ = i;
        apply();
    }
    IdType subscribeForChange(std::function<void()> action) override
    {
        std::lock_guard<std::mutex> lk(subscriberMock_);
        auto id = idPool_.getId();
        subscribers_.push_back({id, action});
        return id;
    }
    void unsubscribe(const IdType& id) override
    {
        std::lock_guard<std::mutex> lk(subscriberMock_);
        auto iter = std::find_if(subscribers_.begin(), subscribers_.end(), [id](const auto& pair) { return pair.first == id; });
        subscribers_.erase(iter);
    }
    void operator=(const T& v)
    {
        if (isLocked_)
            return;

        set(v);
    }
    bool operator==(const T& v1) const
    {
        return value_ == v1;
    }
    bool operator!=(const T& v1) const
    {
        return value_ != v1;
    }
    bool operator<(const T& v1) const
    {
        return value_ < v1;
    }
    bool operator>(const T& v1) const
    {
        return value_ > v1;
    }
    constexpr const T& operator*() const
    {
        return get();
    }
    constexpr operator T() const
    {
        return value_;
    }
    constexpr const T* operator->() const
    {
        return std::addressof(value_);
    }
    void lock() override
    {
        isLocked_ = true;
    }
    void unlock() override
    {
        isLocked_ = false;
    }
    virtual bool isLocked() const override
    {
        return isLocked_;
    }

private:
    void updatetValueIndex()
    {
        for (size_t i = 0; i < defaultValues_.size(); ++i)
        {
            if (value_ == defaultValues_[i])
            {
                defaultValueIndex_ = i;
            }
        }
    }

private:
    std::mutex subscriberMock_;
    std::vector<std::pair<IdType, std::function<void()>>> subscribers_;
    Utils::IdPool idPool_;
    T value_;
    size_t defaultValueIndex_;
    std::array<T, magic_enum::enum_count<T>()> defaultValues_;
    bool isLocked_;
};
}  // namespace Params
}  // namespace GameEngine
