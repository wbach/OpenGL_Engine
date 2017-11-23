#pragma once

template<class T>
class SingleTon
{
public:
	static T& Get()
	{
		static T type;
		return type;
	}

private:
	SingleTon() {}
	T type;
};
