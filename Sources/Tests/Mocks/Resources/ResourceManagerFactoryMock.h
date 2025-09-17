#pragma once
#include <GameEngine/Resources/IResourceManagerFactory.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class ResourceManagerFactoryMock : public IResourceManagerFactory
{
public:
    MOCK_METHOD(std::unique_ptr<IResourceManager>, create, (), (override));
};
}  // namespace GameEngine
