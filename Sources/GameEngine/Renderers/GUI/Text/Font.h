#pragma once
#include "Utils.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

static const uint32 m_MaxCharacters = 128;

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
    mat4 transformationMatrix = mat4(1);

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
