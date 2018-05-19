#pragma once
#include "../FrameBuffer.h"

namespace GameEngine
{
	class PostprocessFrameBuffer : public CFrameBuffer
	{
	public:
		PostprocessFrameBuffer(IGraphicsApiPtr api);
		void Init(const wb::vec2ui& size) override;
		void Clean() override;
	};
} // GameEngine
