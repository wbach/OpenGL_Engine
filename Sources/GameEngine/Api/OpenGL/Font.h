#pragma once
#include "Types.h"
#include "Utils.h"

static const uint32 m_MaxCharacters = 128;

struct SCharacter
{
	char c;
	int width;
	int height;
	uint32 id;
};

class CFont
{
public:
	~CFont();
	void Init(const std::string& file_name);
	void Print(const int& x, const int& y, const std::string& fmt) const;

private:
    mat4 transformationMatrix = mat4(1);

    SCharacter characters[m_MaxCharacters];

    int	   quadIndicesSize;
    uint32 quadVao;
	uint32 quadIndices;
	uint32 quadVertex;
	uint32 quadTexCoord;

	uint32 listBase;
    int baseHeight = 100;

    bool isInit = false;
};
