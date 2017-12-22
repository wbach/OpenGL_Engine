#pragma once
#include "../FrameBuffer.h"

class CDefferedFrameBuffer : public CFrameBuffer
{
public:
	void Init(const wb::vec2i& size) override;
	void Clean() override;
};