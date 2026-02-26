#pragma once

#include <Common/Transform.h>
#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>
#include <Types.h>

#include <GameEngine/Scene/Scene.hpp>
#include <vector>

#include "Command.h"

class ChangeParrentCommand : public Command
{
public:
    ChangeParrentCommand(GameEngine::Scene& scene, IdType gameObjectId, IdType oldParent, IdType newParent)
        : scene(scene)
        , object(gameObjectId)
        , oldParent(oldParent)
        , newParent(newParent)
    {
    }

    void redo() override
    {
        scene.ChangeParent(object, newParent);
    }
    void undo() override
    {
        scene.ChangeParent(object, oldParent);
    }

private:
    GameEngine::Scene& scene;
    IdType object;
    IdType oldParent;
    IdType newParent;
};
