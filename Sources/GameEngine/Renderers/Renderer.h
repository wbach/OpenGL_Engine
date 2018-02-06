#pragma once
#include "Types.h"
#include <memory>
#include <vector>

class CFrameBuffer;
class CGameObject;

namespace GameEngine
{
	class Scene;
} // GameEngine

class CRenderer
{
public:
    CRenderer();
    CRenderer(CFrameBuffer* target);
    virtual ~CRenderer();
    virtual void Init() = 0;
    virtual void ReloadShaders();
    virtual void PrepareFrame(GameEngine::Scene* scene) = 0;
    virtual void Render(GameEngine::Scene* scene)       = 0;
    virtual void EndFrame(GameEngine::Scene* scene)     = 0;
    virtual void Subscribe(CGameObject* gameObject);
	virtual void UnSubscribe(CGameObject* gameObject) {}
	virtual void UnSubscribeAll();
    // Debug
    const uint32& GetObjectsPerFrame();
    const uint32& GetVertexPerFrame();

protected:
    // Debug variables
    uint32 rendererObjectPerFrame   = 0;
    uint32 rendererVertixesPerFrame = 0;
    uint32 maxVerices               = 0;

    CFrameBuffer* target = nullptr;
};

typedef std::unique_ptr<CRenderer> RendererPtr;
typedef std::vector<RendererPtr>   RenderersPtrVec;
