#pragma once
#include <unordered_map>
#include <vector>

template<class X, class Y>
class BidirectionalUnorderedMap
{
public:
	BidirectionalUnorderedMap() {}
	BidirectionalUnorderedMap(std::vector<std::pair<X, Y>> insert)
	{
		for (const auto& p : insert)
		{
			Insert(p.first, p.second);
		}
	}
	void Insert(const X& x, const Y& y)
	{
		map1[x] = y;
		map2[y] = x;
	}

	const X& operator[] (const Y& y) 
	{
		return map2[y];
	}
	const Y& operator[] (const X& x)
	{
		return map1[x];
	}

	const std::unordered_map<X, Y>& GetXY()
	{
		return map1;
	}
	const std::unordered_map<Y, X>& GetYX()
	{
		return map2;
	}
private:
	std::unordered_map<X, Y> map1;
	std::unordered_map<Y, X> map2;
};