#pragma once
#include <GL/glew.h>
#include "../../Utils/Types.h"

struct SImage
{
	GLubyte* m_Data = nullptr;
	uint m_Width = 0, m_Height = 0;

	~SImage()
	{
		if (m_Data == nullptr)
			delete m_Data;
	}
};