#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include "../OpenGLObject.h"
#include "Image.h"

namespace TextureFlip
{
	enum Type {
		NONE = 0,
		VERTICAL,
		HORIZONTAL,
		BOTH
	};
}

class CTexture : public COpenGLObject
{
public:
	CTexture() {}
	CTexture(const std::string& file, const std::string& filepath);
	virtual ~CTexture()
	{
		if (!m_IsInit)
			return;
		glDeleteTextures(1, &m_Id);
	}

	virtual void OpenGLLoadingPass() = 0;
	const GLuint& GetId() const { return m_Id; }
	const std::string GetFileName() { return m_Filename; }
	const std::string GetFilPath() { return m_Fullpath; }
protected:
	GLuint m_Id = 0;
	bool m_IsInit = false;

	std::string m_Filename;
	std::string m_Fullpath;
};