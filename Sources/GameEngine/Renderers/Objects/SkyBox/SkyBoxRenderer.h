#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Shaders/SkyBoxShader.h"

class CModel;
class CMesh;
class CTexture;
class CProjection;
struct SMaterial;

namespace GameEngine
{
struct RendererContext;

class SkyBoxRenderer : public IRenderer
{
public:
    SkyBoxRenderer(RendererContext& context);
    virtual void Init() override;
    void Render(Scene* scene);
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void InitMembers(Scene* scene);
    void BindTextures() const;
    void RenderSkyBoxMesh(const CMesh& mesh) const;
    void RenderSkyBoxModel();
    bool CheckModelIsReadyToRender();
    void PrepareShaderBeforeFrameRender(Scene* scene);
    void BindCubeMapTexture(CTexture* texture, int id) const;
    void LoadModel(ResourceManager& resource_manager);
    void CreateDayTextures(ResourceManager& resource_manager);
    void CreateNightTextures(ResourceManager& resource_manager);
    void PrepareToRendering(Scene* scene);
    void EndRendering();
    void InitShader();

private:
    RendererContext& context_;
    CSkyBoxShader shader;
    CModel* model;
    CTexture* dayTexture;
    CTexture* nightTexture;
    ResourceManager resourceManager;  // TO DO: remove when creat texutres will be outsiede
    vec4 clipPlane;
};
}  // GameEngine
