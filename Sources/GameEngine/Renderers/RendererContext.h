#pragma once
#include "Types.h"
#include <memory>

class CShadowFrameBuffer;

namespace GameEngine
{
	struct RendererContext
	{
		std::shared_ptr<CShadowFrameBuffer> shadowsFrameBuffer;
		mat4 toShadowMapZeroMatrix_;
	};
}