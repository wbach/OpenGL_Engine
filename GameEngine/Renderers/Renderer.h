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
	CRenderer(std::weak_ptr<CFrameBuffer> target) : m_Target(target){}
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
    uint m_RendererObjectPerFrame = 0;
	uint m_RendererVertixesPerFrame = 0;
	uint m_MaxVerices = 0;

	std::weak_ptr<CFrameBuffer> m_Target;
};
