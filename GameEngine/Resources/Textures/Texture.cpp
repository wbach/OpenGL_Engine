#include "Texture.h"

CTexture::CTexture(const std::string & file, const std::string & filepath, bool applySizeLimit)
	: filename(file)
	, fullpath(filepath)
	, applySizeLimit(applySizeLimit)
{
}
CTexture::~CTexture()
{
	if (!isInit)
		return;
	glDeleteTextures(1, &id);
}