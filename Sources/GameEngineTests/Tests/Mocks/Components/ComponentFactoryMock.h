#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Components/IComponentFactory.h"
#include "Types.h"

namespace GameEngine
{
class ComponentFactoryMock : public Components::IComponentFactory
{
public:
    MOCK_METHOD2(CreateImpl, std::unique_ptr<Components::IComponent>(Components::ComponentsType, GameObject&));

private:
    virtual std::unique_ptr<Components::IComponent> Create(Components::ComponentsType type, GameObject& ptr) override
    {
        return CreateImpl(type, ptr);
    }
};

}  // namespace GameEngine
