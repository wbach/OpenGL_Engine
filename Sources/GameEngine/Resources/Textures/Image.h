#pragma once
#include <GL/glew.h>
#include "Types.h"
#include <vector>
#include <memory>

struct SImage
{
	std::vector<GLubyte> data;
	std::vector<float> floatData;
	
	uint32 width = 0;
	uint32 height = 0;
};

typedef std::shared_ptr<SImage> SImagePtr;
