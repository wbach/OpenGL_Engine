#pragma once

namespace wb
{
	template<class T>
	class optional
	{
	public:
		optional() = default;
		optional(const T& v)
			: hasValue(true)
			, value(v)
		{
		}

		optional(const optional<T>& v)
			: hasValue(v.hasValue)
			, value(v.value)
		{
		}

		bool operator==(const T& v) const
		{
			return value == v;
		}

		bool operator==(const optional<T>& v) const
		{
			if (hasValue && v.hasValue)
				return value == v.value;

			//both uninitialized
			if (!hasValue && !v.hasValue)
				return true;

			return false;
		}

		bool operator!() const
		{
			return !hasValue;
		}
		operator bool() const
		{
			return hasValue;
		}

		bool operator>(const T& v) const
		{
			return value > v.value;
		}

		bool operator<(const T& v) const
		{
			return value < v.value;
		}
		bool operator>=(const T& v) const
		{
			return value >= v.value;
		}
		bool operator<=(const T& v) const
		{
			return value <= v.value;
		}
		optional operator=(const T& v)
		{
			this->value = v;
			this->hasValue = true;
			return *this;
		}
		optional & operator++() {
			++value;
			return *this;
		}
		optional operator+ (const T &v)
		{
			optional o;
			o.value = value + v;
			o.hasValue = hasValue;
			return o;
		}
		optional operator- (const T &v)
		{
			optional o;
			o.value = value + v.value;
			o.hasValue = hasValue;
			return o;
		}
		optional operator* (const T &v)
		{
			optional o;
			o.value = value * v;
			o.hasValue = hasValue;
			return o;
		}
		optional operator/ (const T &v)
		{
			optional o;
			o.value = value / v;
			o.hasValue = hasValue;
			return o;
		}
		const T& GetValue() const
		{
			return value;
		}
	private:
		bool hasValue = false;
		T value;
	};
}