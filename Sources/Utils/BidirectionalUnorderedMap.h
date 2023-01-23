#pragma once
#include <unordered_map>
#include <vector>
#include <cstddef>

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

    size_t Count(X x) const
    {
        return map1.count(x);
    }

    size_t Count(Y y) const
    {
        return map2.count(y);
    }

    size_t Size() const
    {
        return map1.size();
    }

private:
    std::unordered_map<X, Y> map1;
    std::unordered_map<Y, X> map2;
};
