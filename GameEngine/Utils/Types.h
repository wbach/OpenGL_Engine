#pragma once
#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif

#include <string>

typedef unsigned int uint;
typedef unsigned char uchar;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

namespace wb
{
	struct vec2i
	{
		int x;
		int y;
	};

	struct vec3i
	{
		int x;
		int y;
		int z;

		bool operator==(const vec3i& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}
		//To my model map find
		bool operator<(const vec3i& v) const {
			return x != v.x || y != v.y || z != v.z;
		};
	};

	std::string to_string(const vec2i& v);
	std::string to_string(const vec3i& v);
}