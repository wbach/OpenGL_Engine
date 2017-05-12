#pragma once
#include "../Renderer.h"
#include "Shaders/SkyBoxShader.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"
#include "../../Resources/Models/Model.h"

class CSkyBoxRenderer : public CRenderer
{
public:
    CSkyBoxRenderer(SProjection* projection_matrix, std::weak_ptr<CFrameBuffer> framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(CScene* scene) override;
    virtual void Render(CScene* scene) override;
    virtual void EndFrame(CScene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;
private:
	void InitMembers(CScene* scene);
	void BindTextures(const SMaterial& material) const;
    CSkyBoxShader   m_Shader;
    CModel*         m_Model;
	CTexture*		m_DayTexture;
	CTexture*		m_NightTexture;

    SProjection*	m_ProjectionMatrix;
    glm::vec4	m_ClipPlane;
};

