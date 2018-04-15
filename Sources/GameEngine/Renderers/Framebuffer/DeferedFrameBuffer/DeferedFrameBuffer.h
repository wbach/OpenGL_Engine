#pragma once
#include "../FrameBuffer.h"

class CDefferedFrameBuffer : public CFrameBuffer
{
public:
	CDefferedFrameBuffer(GameEngine::IGraphicsApiPtr api);
	void Init(const wb::vec2ui& size) override;
	void Clean() override;
};
