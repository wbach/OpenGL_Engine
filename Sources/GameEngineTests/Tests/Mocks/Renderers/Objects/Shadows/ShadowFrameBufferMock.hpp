#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Renderers/Objects/Shadows/IShadowFrameBuffer.hpp"
#include "Types.h"

namespace GameEngine
{
class ShadowFrameBufferMock : public IShadowFrameBuffer
{
public:
    MOCK_METHOD0(BindFBO, void());
    MOCK_CONST_METHOD0(UnbindFrameBuffer, void());
    MOCK_METHOD0(InitialiseFrameBuffer, void());
    MOCK_CONST_METHOD0(GetShadowMap, uint32());
};
}  // GameEngine
