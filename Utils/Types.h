#pragma once
#include <glm/glm.hpp>
#include <string>
#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif

typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef int int32;

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

namespace wb
{
	struct vec2i
	{
		int32 x;
		int32 y;

		vec2i() {}
		vec2i(int32 x, int32 y) 
			: x(x)
			, y(y)
		{}
	};

	struct vec3i
	{
		int32 x;
		int32 y;
		int32 z;

		bool operator==(const vec3i& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}
		//To my model map find
		bool operator<(const vec3i& v) const
		{
			return x != v.x || y != v.y || z != v.z;
		};
	};

	std::string to_string(const vec2i& v);
	std::string to_string(const vec3i& v);
}

typedef wb::vec2i vec2i;
typedef wb::vec3i vec3i;

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::vec4 vec4;

typedef glm::mat3 mat3;
typedef glm::mat4 mat4;


