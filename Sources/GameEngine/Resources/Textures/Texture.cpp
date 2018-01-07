#include "Texture.h"
#include "Logger/Log.h"

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
	Log("Delete " + fullpath +  ", texture id: " + std::to_string(id));
	glDeleteTextures(1, &id);
}