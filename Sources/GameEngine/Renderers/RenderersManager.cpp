#include "RenderersManager.h"
#include "SimpleRenderer.h"
#include "FullRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GUI/Text/GuiText.h"
#include "GUI/Texutre/GuiTexture.hpp"
#include "../Engine/Configuration.h"
#include "../Engine/AplicationContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
	namespace Renderer
	{
		RenderersManager::RenderersManager()
		{

		}
		const CProjection& RenderersManager::GetProjection() const
		{
			return projection_;
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
		void RenderersManager::RenderScene(CScene* scene)
		{
			if (scene == nullptr)
				return;

			for (auto& renderer : renderers_)
				Render(scene, renderer.get());
		}
		void RenderersManager::Subscribe(CGameObject* gameObject)
		{
			for (auto& renderer : renderers_)
				renderer->Subscribe(gameObject);
		}
		void RenderersManager::UnSubscribeAll()
		{
			for (auto& r : renderers_)
				r->UnSubscribeAll();
		}
		SGuiTextElement& RenderersManager::GuiText(const std::string & label)
		{
			return guiContext_.texts->texts[label];
		}
		Gui::GuiTextureElement& RenderersManager::GuiTexture(const std::string & label)
		{
			return guiContext_.texures->guiTextures_[label];
		}
		void RenderersManager::Render(CScene* scene, CRenderer* renderer)
		{
			std::lock_guard<std::mutex>(SingleTon<SAplicationContext>::Get().renderingMutex);
			renderer->PrepareFrame(scene);
			renderer->Render(scene);
			renderer->EndFrame(scene);
		}

	} // Renderer
} // GameEngine