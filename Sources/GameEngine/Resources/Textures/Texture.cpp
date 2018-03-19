#include "Texture.h"
#include "Logger/Log.h"

CTexture::CTexture(GameEngine::IGraphicsApiPtr graphicsApi, const std::string & file, const std::string & filepath, bool applySizeLimit)
	: graphicsApi_(graphicsApi)
	, filename(file)
	, fullpath(filepath)
	, applySizeLimit(applySizeLimit)
{
}
CTexture::~CTexture()
{
	if (!isInit)
		return;
	Log("Delete " + fullpath +  ", texture id: " + std::to_string(id));
	
	graphicsApi_->DeleteObject(id);
}