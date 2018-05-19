#pragma once
#include "Shaders/SkyBoxShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"

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
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void ReloadShaders() override;

	private:
		void InitMembers(Scene* scene);
		void BindTextures() const;
		void RenderSkyBoxMesh(const CMesh& mesh) const;
		void RenderSkyBoxModel();
		bool CheckModelIsReadyToRender();
		void PrepareShaderBeforeFrameRender(Scene* scene);
		void BindCubeMapTexture(CTexture* texture, int id) const;
		void LoadModel(CResourceManager& resource_manager);
		void CreateDayTextures(CResourceManager& resource_manager);
		void CreateNightTextures(CResourceManager& resource_manager);
		void PrepareToRendering(Scene* scene);
		void EndRendering();
		void InitShader();

	private:
		RendererContext& context_;
		CSkyBoxShader shader;
		CModel* model;
		CTexture* dayTexture;
		CTexture* nightTexture;
		CResourceManager resourceManager; // TO DO: remove when creat texutres will be outsiede
		vec4 clipPlane;
	};
} // GameEngine

