#pragma once
#include "Types.h"
#include <memory>

class CProjection;
class IGraphicsApi;
class CFrameBuffer;
class CShadowFrameBuffer;
class PostprocessFrameBuffer;

namespace GameEngine
{
	struct RendererContext
	{
		mat4 toShadowMapZeroMatrix_;
		CProjection* projectionMatrix_;
		std::shared_ptr<IGraphicsApi> graphicsApi_;
		std::shared_ptr<CFrameBuffer> defferedFrameBuffer_;
		std::shared_ptr<CShadowFrameBuffer> shadowsFrameBuffer_;
	};
}