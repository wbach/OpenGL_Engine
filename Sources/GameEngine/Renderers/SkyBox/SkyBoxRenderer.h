#pragma once
#include "../Renderer.h"
#include "Shaders/SkyBoxShader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CModel;
class CMesh;
class CTexture;
class CProjection;
struct SMaterial;

class CSkyBoxRenderer : public CRenderer
{
public:
    CSkyBoxRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene* scene) override;
    virtual void Render(GameEngine::Scene* scene) override;
    virtual void EndFrame(GameEngine::Scene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;

private:
    void InitMembers(GameEngine::Scene* scene);
    void BindTextures() const;
    void RenderSkyBoxMesh(const CMesh& mesh) const;
    void RenderSkyBoxModel();
    bool CheckModelIsReadyToRender();
    void PrepareShaderBeforeFrameRender(GameEngine::Scene* scene);
    void BindCubeMapTexture(CTexture* texture, int id) const;
    bool SetTarget();
    void LoadModel(CResourceManager& resource_manager);
    void CreateDayTextures(CResourceManager& resource_manager);
    void CreateNightTextures(CResourceManager& resource_manager);
    void PrepareToRendering(GameEngine::Scene* scene);
    void EndRendering();

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    CSkyBoxShader shader;
    CModel* model;
    CTexture* dayTexture;
    CTexture* nightTexture;
	CResourceManager resourceManager; // TO DO: remove when creat texutres will be outsiede
    CProjection* projectionMatrix;
    vec4 clipPlane;
};
