#include "Texture.h"

CTexture::CTexture(const std::string & file, const std::string & filepath, bool applySizeLimit)
	: filename(file)
	, m_Fullpath(filepath)
	, applySizeLimit(applySizeLimit)
{
}
CTexture::~CTexture()
{
	if (!m_IsInit)
		return;
	glDeleteTextures(1, &m_Id);
}