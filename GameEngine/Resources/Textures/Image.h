#pragma once
#include <GL/glew.h>
#include "Types.h"

struct SImage
{
    GLubyte* data = nullptr;
    uint32 width = 0, height = 0;

	~SImage()
	{
        if (data == nullptr)
            delete data;
	}
};
