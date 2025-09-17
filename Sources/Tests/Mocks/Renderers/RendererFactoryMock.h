#pragma once
#include <GameEngine/Renderers/IRendererFactory.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class RendererFactoryMock : public IRendererFactory
{
public:
    MOCK_METHOD(std::unique_ptr<IRenderer>, create, (RendererContext&), (const, override));
};
}  // namespace GameEngine
