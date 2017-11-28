#pragma once
#include "../Renderer.h"
#include "Shaders/SkyBoxShader.h"

class CModel;
class CMesh;
class CTexture;
class CProjection;
class CResourceManager;
struct SMaterial;

class CSkyBoxRenderer : public CRenderer
{
public:
    CSkyBoxRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(CScene* scene) override;
    virtual void Render(CScene* scene) override;
    virtual void EndFrame(CScene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;

private:
    void InitMembers(CScene* scene);
    void BindTextures(const SMaterial& material) const;
    void RenderSkyBoxMesh(const CMesh& mesh) const;
    void RenderSkyBoxModel();
    bool CheckModelIsReadyToRender();
    void PrepareShaderBeforeFrameRender(CScene* scene);
    void BindCubeMapTexture(CTexture* texture, int id) const;
    bool SetTarget();
    void LoadModel(CResourceManager& resource_manager);
    void CreateDayTextures(CResourceManager& resource_manager);
    void CreateNightTextures(CResourceManager& resource_manager);
    void PrepareToRendering(CScene* scene);
    void EndRendering();
    CTexture* LoadCubeMapTexture(CResourceManager& resource_manager, std::vector<std::string> textures_files);

private:
    CSkyBoxShader shader;
    CModel* model;
    CTexture* dayTexture;
    CTexture* nightTexture;

    CProjection* projectionMatrix;
    vec4 clipPlane;
};
