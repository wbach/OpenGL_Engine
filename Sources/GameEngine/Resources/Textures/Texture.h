#pragma once
#include "Types.h"
#include "TextureFlip.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "../OpenGLObject.h"
#include <string>

class CTexture : public COpenGLObject
{
public:
	CTexture() {}
    CTexture(GameEngine::IGraphicsApiPtr graphicsApi)
		: graphicsApi_(graphicsApi)
    {
    }
    CTexture(GameEngine::IGraphicsApiPtr graphicsApi, const std::string& file, const std::string& filepath, bool applySizeLimit = true);
    virtual ~CTexture();
	virtual void OpenGLLoadingPass() {}
	inline uint32 GetId() const;
	inline const std::string GetFileName();
	inline const std::string GetFilPath();
	inline void SetExistId(uint32 i);
	inline bool IsInitialized() const;
	inline float GetTextureXOffset(uint32 textureIndex);
	inline float GetTextureYOffset(uint32 textureIndex);
	inline vec2 GetTextureOffset(uint32 textureIndex);

public:
	uint32 numberOfRows = 1;

protected:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    std::string filename;
    std::string fullpath;

    bool applySizeLimit = true;

	uint32 id   = 0;
    bool isInit = false;
};

uint32 CTexture::GetId() const
{
	return id;
}
const std::string CTexture::GetFileName()
{
	return filename;
}
const std::string CTexture::GetFilPath()
{
	return fullpath;
}
void CTexture::SetExistId(uint32 i)
{
	id = i;
}
bool CTexture::IsInitialized() const
{
	return isInit;
}
float CTexture::GetTextureXOffset(uint32 textureIndex)
{
	int column = textureIndex % numberOfRows;
	return static_cast<float>(column) / static_cast<float>(numberOfRows);
}
float CTexture::GetTextureYOffset(uint32 textureIndex)
{
	int row = textureIndex / numberOfRows;
	return static_cast<float>(row) / static_cast<float>(numberOfRows);

}
vec2 CTexture::GetTextureOffset(uint32 textureIndex)
{
	return vec2(GetTextureXOffset(textureIndex), GetTextureYOffset(textureIndex));
}
