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

private:
    glm::mat4 transformationMatrix = glm::mat4(1);

    SCharacter characters[m_MaxCharacters];

    int	   quadIndicesSize;
    GLuint quadVao;
    GLuint quadIndices;
    GLuint quadVertex;
    GLuint quadTexCoord;

    GLuint listBase;
    int baseHeight = 100;

    bool isInit = false;
};
