#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
class ShaderFactoryMock : public IShaderFactory
{
public:
    virtual std::unique_ptr<IShaderProgram> create(Shaders s) override
    {
        return std::unique_ptr<IShaderProgram>(createImpl(s));
    }
    MOCK_METHOD1(createImpl, IShaderProgram*(Shaders));
};
}  // namespace GameEngine
