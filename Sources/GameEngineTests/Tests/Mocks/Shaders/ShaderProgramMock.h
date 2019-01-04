#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
class ShaderProgramMock : public IShaderProgram
{
public:
    MOCK_METHOD0(Init, void());
    MOCK_METHOD0(Reload, void());
    MOCK_METHOD1(SetFiles, void(const ShadersFiles&));

	MOCK_CONST_METHOD0(IsReady, bool());
    MOCK_CONST_METHOD0(IsReadyToLoad, bool());

    MOCK_CONST_METHOD0(Start, void());
    MOCK_CONST_METHOD0(Stop, void());

	MOCK_CONST_METHOD2(Load, void(uint32, bool));
    MOCK_CONST_METHOD2(Load, void(uint32, int));
    MOCK_CONST_METHOD2(Load, void(uint32, uint32));
    MOCK_CONST_METHOD2(Load, void(uint32, float));
    MOCK_CONST_METHOD2(Load, void(uint32, const vec2ui&));
    MOCK_CONST_METHOD2(Load, void(uint32, const vec2&));
    MOCK_CONST_METHOD2(Load, void(uint32, const vec3&));
    MOCK_CONST_METHOD2(Load, void(uint32, const vec4&));
    MOCK_CONST_METHOD2(Load, void(uint32, const mat3&));
    MOCK_CONST_METHOD2(Load, void(uint32, const mat4&));
    MOCK_CONST_METHOD2(Load, void(uint32, const std::vector<float>&));
    MOCK_CONST_METHOD2(Load, void(uint32, const std::vector<vec3>&));
    MOCK_CONST_METHOD2(Load, void(uint32, const std::vector<mat4>&));

    MOCK_CONST_METHOD1(GetUniformLocation, uint32(const std::string&));
};
}  // namespace GameEngine
