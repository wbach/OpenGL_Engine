#pragma once
#include <freetype2/ft2build.h>
#include <freetype2/freetype.h>
#include <freetype2/ftglyph.h>
#include <freetype2/ftoutln.h>
#include <freetype2/fttrigon.h>
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "../../../Utils/Utils.h"

static const uint m_MaxCharacters = 128;

struct SCharacter
{
	char c;
	int width;
	int height;
	GLuint id;
};

class CFont
{
public:
	~CFont();
	void Init(const std::string& file_name, const wb::vec2i& window_size);
	void Print(const int& x, const int& y, const std::string& fmt) const;
private:
	void CreateList(FT_Face face, char ch);

	glm::mat4 m_TransformationMatrix = glm::mat4(1);

	SCharacter m_Characters[m_MaxCharacters];

	int	   m_QuadIndicesSize;
	GLuint m_QuadVao;
	GLuint m_QuadIndices;
	GLuint m_QuadVertex;
	GLuint m_QuadTexCoord;

	GLuint m_ListBase;
	int m_BaseHeight = 100;	

	bool m_IsInit = false;
};