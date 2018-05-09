#pragma once
#include "PostprocessingRendererTypes.h"

namespace GameEngine
{
struct IPostprocessingRenderersFactory
{
	virtual ~IPostprocessingRenderersFactory() {}
	virtual void Create(PostprocessingRendererType type) = 0;
};
}
