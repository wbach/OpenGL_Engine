#pragma once
#include "Types.h"
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
    const uint32& GetObjectsPerFrame();
	const uint32& GetVertexPerFrame();

protected:
    //Debug variables
    uint32 rendererObjectPerFrame = 0;
    uint32 rendererVertixesPerFrame = 0;
    uint32 maxVerices = 0;

    CFrameBuffer* target = nullptr;
};
