#pragma once
#include "Types.h"
#include "RendererFunctionType.h"
#include <memory>
#include <functional>

class CProjection;
class CFrameBuffer;
class CShadowFrameBuffer;
class PostprocessFrameBuffer;

namespace GameEngine
{
	class IGraphicsApi;

	struct RendererContext
	{
		RendererContext() {}
		RendererContext(CProjection* p, const std::shared_ptr<IGraphicsApi>& api,
			const std::shared_ptr<CFrameBuffer>& defferedBuffer, const std::shared_ptr<CShadowFrameBuffer>& shadowBuffer
		, std::function<void(RendererFunctionType, RendererFunction)> registerFunction) 
			: projection_(p)
			, graphicsApi_(api)
			, defferedFrameBuffer_(defferedBuffer)
			, shadowsFrameBuffer_(shadowBuffer)
			, registerFunction_(registerFunction)
		{}
		CProjection* projection_;
		mat4 toShadowMapZeroMatrix_;
		std::shared_ptr<IGraphicsApi> graphicsApi_;
		std::shared_ptr<CFrameBuffer> defferedFrameBuffer_;
		std::shared_ptr<CShadowFrameBuffer> shadowsFrameBuffer_;
		std::function<void (RendererFunctionType, RendererFunction)> registerFunction_;
	};
} // GameEngine
#define __RegisterRenderFunction__(x, y) context_.registerFunction_(x, std::bind(&y, this, std::placeholders::_1))
