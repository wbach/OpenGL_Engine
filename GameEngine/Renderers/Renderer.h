#pragma once
#include "../Utils/Types.h"
#include <memory>

class CScene;
class CFrameBuffer;
class CGameObject;

class CRenderer
{
public:
	CRenderer() {}
    CRenderer(CFrameBuffer* target) : target(target){}
	virtual ~CRenderer() {}
    virtual void Init() = 0 ;
    virtual void PrepareFrame(CScene* scene) = 0;
    virtual void Render(CScene* scene) = 0;
	virtual void EndFrame(CScene* scene) = 0;
	virtual void Subscribe(CGameObject* gameObject) {}
    //Debug
    const uint& GetObjectsPerFrame();
	const uint& GetVertexPerFrame();

protected:
    //Debug variables
    uint rendererObjectPerFrame = 0;
    uint rendererVertixesPerFrame = 0;
    uint maxVerices = 0;

    CFrameBuffer* target = nullptr;
};
