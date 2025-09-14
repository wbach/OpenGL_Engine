#pragma once
#include <algorithm>
#include <functional>
#include "Types.h"

namespace Utils
{
template <class T>
bool ValidateRange(const T& c, std::uint32_t id)
{
    return (id >= 0) and (id < c.size());
}
template <class Container>
const typename Container::value_type* get(const Container& container, const typename Container::key_type& key)
{
    auto it = container.find(key);
    if (it != container.end())
    {
        return &(*it);
    }
    return nullptr;
}
template <class Container>
typename Container::value_type* get(Container& container, const typename Container::value_type& value)
{
    auto it = std::find(container.begin(), container.end(), value);
    if (it != container.end())
    {
        return &(*it);
    }
    return nullptr;
}
template <class Container>
typename Container::value_type* get(Container& container, std::function<bool(const typename Container::value_type&)> predicate)
{
    auto it = std::find_if(container.begin(), container.end(), predicate);
    if (it != container.end())
    {
        return &(*it);
    }
    return nullptr;
}
template <class Container>
bool contains(const Container& container, std::function<bool(const typename Container::value_type&)> predicate)
{
    auto it = std::find_if(container.begin(), container.end(), predicate);
    return (it != container.end());
}
}  // namespace Utils
