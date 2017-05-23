#pragma once
#include <GL/glew.h>
#include "../../Utils/Types.h"

struct SImage
{
    GLubyte* data = nullptr;
    uint width = 0, height = 0;

	~SImage()
	{
        if (data == nullptr)
            delete data;
	}
};
