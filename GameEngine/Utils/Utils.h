#pragma once
#include "Types.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <sstream>

namespace Utils
{
	template <typename T>
	T clamp(const T& n, const T& lower, const T& upper) {
		return std::max(lower, std::min(n, upper));
	}

	template<class type>
	float ToRadians(type a)
	{
		return static_cast<float>(a) * static_cast<float>(M_PI) / 180.0f;
	}

	template<class type>
	type ToDegrees(type a)
	{
		return a * 180.0f / static_cast<float>(M_PI);
	}

	template<class type>
	type KmToMs(type a)
	{
		return a * 0.277777778f;
	}

	template <typename T> std::string ToStr(const T& t)
	{
		std::ostringstream os;
		os << t;
		return os.str();
	}
	std::vector<std::string> SplitString(const std::string& s, char split_char);
	std::string ReadFile(const std::string& file_name);
	void GetFileAndPath(const std::string& full, std::string& filename, std::string& path);
	std::string ConvertToRelativePath(std::string path);	
	int NextP2(int a);
	bool CheckFileExist(const std::string& file);
}
