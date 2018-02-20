#include "RenderersManager.h"

#include "FullRenderer.h"
#include "SimpleRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GUI/Text/GuiText.h"
#include "GUI/Texutre/GuiTexture.hpp"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
	struct RenderAsLine
	{
		RenderAsLine(bool use)
			: use(use)
		{
			if (use)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		~RenderAsLine()
		{
			if (use)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		bool use;
	};

	namespace Renderer
	{
		RenderersManager::RenderersManager()
			: markToReloadShaders_(false)
		{

		}
		const CProjection& RenderersManager::GetProjection() const
		{
			return projection_;
		}
		void RenderersManager::UpdateCamera(Scene * scene)
		{
			auto camera = scene->GetCamera();

			if (camera == nullptr)
				return;

			camera->CalculateInput();
			camera->Move();
			camera->UpdateMatrix();
		}
		void RenderersManager::TakeSnapShoots()
		{
			for (auto& obj : dynamincObjects_)
				obj->worldTransform.TakeSnapShoot();
		}
		void RenderersManager::Init()
		{
			InitProjection();
			InitMainRenderer();
			InitGuiRenderer();

			for (auto& renderer : renderers_)
				renderer->Init();
		}
		void RenderersManager::InitProjection()
		{
			auto& conf = EngineConf;
			projection_ = conf.resolution;
		}
		void RenderersManager::InitMainRenderer()
		{
			if (!renderers_.empty())
				return;

			auto rendererType = EngineConf.rendererType;

			if (rendererType == SEngineConfiguration::RendererType::FULL_RENDERER)
				renderers_.emplace_back(new FullRenderer(&projection_));
			else
				renderers_.emplace_back(new SimpleRenderer(&projection_));

		}
		void RenderersManager::InitGuiRenderer()
		{
			guiContext_.renderer = new CGUIRenderer();
			guiContext_.texts = new CGuiText("GUI/consola.ttf", projection_.GetWindowSize());
			guiContext_.texures = new Renderer::Gui::CGuiTexture();
			guiContext_.renderer->AddElement(guiContext_.texures);
			guiContext_.renderer->AddElement(guiContext_.texts);
			renderers_.emplace_back(guiContext_.renderer);
		}
		void RenderersManager::RenderScene(Scene* scene)
		{
			if (scene == nullptr)
				return;

			TakeSnapShoots();
			UpdateCamera(scene);
			ReloadShadersExecution();

			RenderAsLine lineMode(renderAsLines.load());

			for (auto& renderer : renderers_)
				Render(scene, renderer.get());
		}
		void RenderersManager::ReloadShaders()
		{
			markToReloadShaders_.store(true);
		}
		void RenderersManager::ReloadShadersExecution()
		{
			if (!markToReloadShaders_.load())
				return;

			for (auto& renderer : renderers_)
				renderer->ReloadShaders();

			markToReloadShaders_.store(false);
		}
		void RenderersManager::Subscribe(CGameObject* gameObject)
		{
			if (gameObject == nullptr)
				return;

			if (gameObject->worldTransform.isDynamic_)
				dynamincObjects_.push_back(gameObject);

			for (auto& renderer : renderers_)
				renderer->Subscribe(gameObject);
		}
		void RenderersManager::UnSubscribe(CGameObject* gameObject)
		{
			for (auto iter = dynamincObjects_.begin(); iter != dynamincObjects_.end();)
			{
				if (gameObject->GetId())
				{
					iter = dynamincObjects_.erase(iter);
					break;
				}
				else
				{
					++iter;
				}
			}

			for (auto& r : renderers_)
				r->UnSubscribe(gameObject);
		}
		void RenderersManager::UnSubscribeAll()
		{
			for (auto& r : renderers_)
				r->UnSubscribeAll();

			dynamincObjects_.clear();
		}
		void RenderersManager::SwapLineFaceRender()
		{
			renderAsLines.store(!renderAsLines.load());
		}
		SGuiTextElement& RenderersManager::GuiText(const std::string & label)
		{
			return guiContext_.texts->texts[label];
		}
		Gui::GuiTextureElement& RenderersManager::GuiTexture(const std::string & label)
		{
			return guiContext_.texures->guiTextures_[label];
		}
		void RenderersManager::Render(Scene* scene, CRenderer* renderer)
		{
			renderer->PrepareFrame(scene);
			renderer->Render(scene);
			renderer->EndFrame(scene);
		}

	} // Renderer
} // GameEngine