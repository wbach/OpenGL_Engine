#pragma once
#include <Types.h>
#include <Utils/IdPool.h>

#include <Mutex.hpp>
#include <memory>
#include <unordered_map>
#include <functional>

#include "ParamToString.h"
#include "IConfigurationParam.h"

namespace GameEngine
{
namespace Params
{
template <class T>
class ConfigurationParam : public IConfigurationParam
{
public:
    ConfigurationParam(IConfigurationParam::Type type, const T& v, const std::vector<T>& defaultValues)
        : type_{type}
        , value_{v}
        , defaultValueIndex_{0}
        , defaultValues_{defaultValues}
    {
    }
    void AddDefaultValues(const std::vector<T>& defaultValues)
    {
        for (const auto& value : defaultValues)
        {
            defaultValues_.push_back(value);
        }
    }
    void AddDefaultValue(const T& value)
    {
        defaultValues_.push_back(value);
    }
    const std::vector<T>& GetDefaultValues() const
    {
        return defaultValues_;
    }
    std::string toString() const override
    {
        return paramToString(value_);
    }
    std::string next() override
    {
        if (not defaultValues_.empty())
        {
            auto result = defaultValues_[defaultValueIndex_++];

            if (defaultValueIndex_ > defaultValues_.size())
            {
                defaultValueIndex_ = 0;
            }

            return paramToString(result);
        }

        return toString();
    }
    std::string previous() override
    {
        if (not defaultValues_.empty())
        {
            auto result = defaultValues_[defaultValueIndex_];

            if (defaultValueIndex_ == 0)
            {
                defaultValueIndex_ = defaultValues_.size() - 1;
            }
            else
            {
                --defaultValueIndex_;
            }
            return paramToString(result);
        }
        return toString();
    }
    void apply() override
    {
        if (not defaultValues_.empty())
        {
            set(defaultValues_[defaultValueIndex_]);
        }
    }
    IConfigurationParam::Type getType() const override
    {
        return type_;
    }
    const T& get() const
    {
        return value_;
    }
    void set(const T& value)
    {
        if (value_ == value)
            return;

        std::lock_guard<std::mutex> lk(subscriberMock_);
        value_ = value;

        for (const auto& subscriber : subscribers_)
        {
            subscriber.second(value_);
        }
    }
    IdType subscribeForChange(std::function<void(const T&)> action)
    {
        std::lock_guard<std::mutex> lk(subscriberMock_);
        auto id = idPool_.getId();
        subscribers_.insert({id, action});
        return id;
    }
    void unsubscribe(const IdType& id)
    {
        std::lock_guard<std::mutex> lk(subscriberMock_);
        subscribers_.erase(id);
    }
    void operator=(const T& v)
    {
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

private:
    std::mutex subscriberMock_;
    std::unordered_map<IdType, std::function<void(const T&)>> subscribers_;
    Utils::IdPool idPool_;
    IConfigurationParam::Type type_;
    T value_;
    size_t defaultValueIndex_;
    std::vector<T> defaultValues_;
};
}  // namespace Params
}  // namespace GameEngine
