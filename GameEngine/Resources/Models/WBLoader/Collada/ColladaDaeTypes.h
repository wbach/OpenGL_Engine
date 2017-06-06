#pragma once
#include <vector>

namespace WBLoader
{
	namespace ColladaTypes
	{
		enum ArrayType
		{
			POSITIONS,
			NORMALS,
			TEXT_COORD,
			UNKNOWN
		};

		struct source
		{
			std::vector<float> floatArray;
		};
	}
}