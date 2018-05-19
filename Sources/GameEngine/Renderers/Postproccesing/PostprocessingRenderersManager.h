#pragma once
#include "Types.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRendererTypes.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Renderers/Postproccesing/IPostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/Framebuffer/Postprocessing/PostproccesFrameBuffer.h"
#include <unordered_map>

namespace GameEngine
{
	class Scene;

	typedef std::vector<PostprocessingRendererPtr> PostprocessingRenderers;

	class PostProcessingManager
	{
	public:
		PostProcessingManager(RendererContext& context);
		~PostProcessingManager();
		void Init();
		void Render(Scene*);
		void ReloadShaders();

	private:
		RendererContext& context_;
		PostprocessFrameBuffer postproccesFrameBuffer_;
		PostprocessingRenderers postProcessingRenderers_;
		std::unique_ptr<IPostprocessingRenderersFactory> factory_;
	};
} // GameEngine
