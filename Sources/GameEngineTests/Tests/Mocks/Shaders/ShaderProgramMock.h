#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
class ShaderProgramMock : public IShaderProgram
{
public:
    MOCK_METHOD0(Init, bool());
    MOCK_METHOD0(Reload, void());
    MOCK_CONST_METHOD0(IsReady, bool());
    MOCK_CONST_METHOD0(IsReadyToLoad, bool());
    MOCK_CONST_METHOD0(Start, void());
    MOCK_CONST_METHOD0(Stop, void());
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, bool value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, int value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, uint32 value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, float value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const vec2ui& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const vec2& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const vec3& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const vec4& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const mat3& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const mat4& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const std::vector<float>& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const std::vector<vec3>& value));
    MOCK_CONST_METHOD2(Load, void(uint32 varIndex, const std::vector<mat4>& value));
    MOCK_CONST_METHOD1(GetUniformLocation, uint32(const std::string& uniformName));
};
}  // namespace GameEngine
