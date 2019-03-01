#pragma once
#include <unordered_map>
#include <vector>

template <class X, class Y>
class BidirectionalUnorderedMap
{
public:
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

    const X& operator[](const Y& y) const
    {
        return map2.at(y);
    }
    const Y& operator[](const X& x) const
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

private:
    std::unordered_map<X, Y> map1;
    std::unordered_map<Y, X> map2;
};
