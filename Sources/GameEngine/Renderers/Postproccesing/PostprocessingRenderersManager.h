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
		void AddEffect(PostprocessingRendererType);

	private:
		RendererContext& context_;
		bool first;
		PostprocessFrameBuffer* activePostProcessFrameBuffer_;
		PostprocessFrameBuffer* passivePostProcessFrameBuffer_;
		PostprocessFrameBuffer postproccesFrameBuffer1_;
		PostprocessFrameBuffer postproccesFrameBuffer2_;
		PostprocessingRenderers postProcessingRenderers_;
		std::unique_ptr<IPostprocessingRenderersFactory> factory_;
	};
} // GameEngine
