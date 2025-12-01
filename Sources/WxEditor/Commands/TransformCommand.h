#pragma once

#include <Common/Transform.h>
#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>
#include <Types.h>

#include <GameEngine/Scene/Scene.hpp>

#include "Command.h"

class TransformCommand : public Command
{
public:
    TransformCommand(GameEngine::Scene& scene, IdType gameObjectId, const common::Transform& old,
                     const common::Transform& newTransform)
        : scene(scene)
        , gameObjectId{gameObjectId}
        , oldTransform{old}
        , newTransform{newTransform}
    {
    }

    void redo() override
    {
        if (auto go = scene.GetGameObject(gameObjectId))
        {
            go->SetLocalTransform(newTransform);
        }
        else
        {
            LOG_WARN << "GameObject with Id" << gameObjectId << " no longer exist!";
        }
    }

    void undo() override
    {
        if (auto go = scene.GetGameObject(gameObjectId))
        {
            go->SetLocalTransform(oldTransform);
        }
        else
        {
            LOG_WARN << "GameObject with Id" << gameObjectId << " no longer exist!";
        }
    }

private:
    GameEngine::Scene& scene;
    IdType gameObjectId;
    common::Transform oldTransform;
    common::Transform newTransform;
};