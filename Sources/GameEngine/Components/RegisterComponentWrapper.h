#pragma once
#include "IComponent.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"

using ComponentReadFunctionRawPtr = GameEngine::Components::IComponent* (*)(GameEngine::Components::ComponentContext&, const TreeNode&, GameEngine::GameObject&);

struct ENGINE_API RegisterComponentWrapper
{
    GameEngine::Components::ComponentTypeID id;
    ComponentReadFunctionRawPtr readFunc;
};
