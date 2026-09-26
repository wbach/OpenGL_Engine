#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

#include "Types.h"

namespace Utils
{
using BlackboardValue = std::variant<bool, int, float, std::string, vec2, vec3, vec4, Quaternion>;

class Blackboard
{
public:
    template <typename T>
    void Set(const std::string& key, const T& value)
    {
        data[key] = value;
    }

    template <typename T>
    std::optional<T> Get(const std::string& key) const
    {
        auto it = data.find(key);
        if (it != data.end())
        {
            if (const T* val = std::get_if<T>(&it->second))
            {
                return *val;
            }
        }
        return std::nullopt;
    }

    bool Has(const std::string& key) const
    {
        return data.find(key) != data.end();
    }

    void Clear(const std::string& key)
    {
        data.erase(key);
    }

private:
    std::unordered_map<std::string, BlackboardValue> data;
};
}  // namespace Utils
