#pragma once
#include "Types.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRendererTypes.h"
#include "GameEngine/Renderers/RendererContext.h"
#include <unordered_map>

namespace GameEngine
{
typedef std::unordered_map<PostprocessingRendererType, PostprocessingRendererPtr> PostprocessingRenderers;

class PostProcessingManager
{
public:
	PostProcessingManager(RendererContext& context);
	void Init();
	void Render();

private:
	RendererContext& context_;
	PostprocessingRenderers postProcessingRenderers_;
};
} // GameEngine
