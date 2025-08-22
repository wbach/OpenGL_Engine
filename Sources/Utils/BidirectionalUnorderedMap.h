#pragma once
#include <cstddef>
#include <unordered_map>
#include <vector>

template <typename X, typename Y, typename HashX = std::hash<X>, typename PredX = std::equal_to<X>,
          typename AllocX = std::allocator<std::pair<const X, Y>>, typename HashY = std::hash<Y>,
          typename PredY = std::equal_to<Y>, typename AllocY = std::allocator<std::pair<const Y, X>>>
class BidirectionalUnorderedMap
{
public:
    using MapType1 = std::unordered_map<X, Y, HashX, PredX, AllocX>;
    using MapType2 = std::unordered_map<Y, X, HashY, PredY, AllocY>;

    BidirectionalUnorderedMap()
    {
    }
    BidirectionalUnorderedMap(std::vector<std::pair<X, Y>> insert)
    {
        for (const auto& p : insert)
        {
            Insert(p.first, p.second);
        }
    }
    void Insert(const X& x, const Y& y)
    {
        map1.insert({x, y});
        map2.insert({y, x});
    }

    X& operator[](const Y& y)
    {
        return map2[y];
    }
    Y& operator[](const X& x)
    {
        return map1[x];
    }

    const X& operator[](const Y& y) const
    {
        return map2.at(y);
    }
    const Y& operator[](const X& x) const
    {
        return map1.at(x);
    }

    const X& At(const Y& y) const
    {
        return map2.at(y);
    }
    const Y& At(const X& x) const
    {
        return map1.at(x);
    }

    const std::unordered_map<X, Y>& GetXY() const
    {
        return map1;
    }
    const std::unordered_map<Y, X>& GetYX() const
    {
        return map2;
    }

    size_t Count(const X& x) const
    {
        return map1.count(x);
    }

    size_t Count(const Y& y) const
    {
        return map2.count(y);
    }

    size_t Size() const
    {
        return map1.size();
    }

    bool keyExist(const X& x) const
    {
        return map1.find(x) != map1.end();
    }

    bool keyExist(const Y& y) const
    {
        return map2.find(y) != map2.end();
    }

    typename MapType1::iterator find(const X& x)
    {
        return map1.find(x);
    }

    typename MapType1::const_iterator find(const X& x) const
    {
        return map1.find(x);
    }

    typename MapType2::iterator find(const Y& y)
    {
        return map2.find(y);
    }

    typename MapType2::const_iterator find(const Y& y) const
    {
        return map2.find(y);
    }

    Y* Get(const X& x)
    {
        auto iter = map1.find(x);
        if (iter != map1.end())
        {
            return &iter->second;
        }
        return nullptr;
    }

    X* Get(const Y& y)
    {
        auto iter = map2.find(y);
        if (iter != map2.end())
        {
            return &iter->second;
        }
        return nullptr;
    }

    void Clear()
    {
        map1.clear();
        map2.clear();
    }

    void Erase(const X& x)
    {
        if (auto y = Get(x))
        {
            map2.erase(*y);
        }
        map1.erase(x);
    }

    void Erase(const Y& y)
    {
        if (auto x = Get(y))
        {
            map1.erase(*x);
        }
        map2.erase(y);
    }

private:
    MapType1 map1;
    MapType2 map2;
};
