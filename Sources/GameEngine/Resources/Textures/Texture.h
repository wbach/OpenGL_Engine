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
	uint32 GetId() const
    {
        return id;
    }
    const std::string GetFileName()
    {
        return filename;
    }
    const std::string GetFilPath()
    {
        return fullpath;
    }
	void SetExistId(uint32 i)
	{
		id = i;
	}
	bool IsInitialized() const
	{
		return isInit;
	}

protected:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    std::string filename;
    std::string fullpath;

    bool applySizeLimit = true;

	uint32 id   = 0;
    bool isInit = false;
};
