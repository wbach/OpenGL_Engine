#pragma once
#include "Renderer.h"
#include "../Shaders/Deffered/LightPassShader.h"
#include "../Resources/SimpleModels/Quad.h"
#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

class CLightPassRenderer : public CRenderer
{
public:
	CLightPassRenderer(SProjection* projection, std::weak_ptr<CFrameBuffer> frambuffer);
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
private:
	CLightPassShader m_Shader;
	SProjection* m_Projection;
	SSimpleQuad m_Quad;
};