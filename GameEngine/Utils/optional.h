#pragma once

template<class T>
class optional
{
public:
	optional()
	{
	}
	optional(T t)
		: value(t)
		, isSet(true)
	{
	}
	optional& optional::operator=(T arg)
	{
		this->value = arg;
		this->isSet = true;
		return *this;
	}
	const T& GetValue() 
	{
		return value;
	}
	const T& operator*()
	{
		return value;
	}
private:
	bool isSet = false;
	T value;
};