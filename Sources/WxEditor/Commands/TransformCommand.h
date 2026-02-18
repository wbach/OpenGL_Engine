#pragma once

#include <Common/Transform.h>
#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>
#include <Types.h>

#include <GameEngine/Scene/Scene.hpp>
#include <vector>

#include "Command.h"

class TransformCommand : public Command
{
public:
    struct Context
    {
        IdType gameObjectId;
        common::Transform oldTransform;
        common::Transform newTransform;
    };

    TransformCommand(GameEngine::Scene& scene, IdType gameObjectId, const common::Transform& old,
                     const common::Transform& newTransform)
        : scene(scene)
    {
        contexts.push_back(Context{.gameObjectId = gameObjectId, .oldTransform = old, .newTransform = newTransform});
    }

    TransformCommand(GameEngine::Scene& scene, std::vector<Context>&& contexts)
        : scene(scene)
        , contexts(std::move(contexts))
    {
    }

    void redo() override
    {
        for (auto& context : contexts)
        {
            if (auto go = scene.GetGameObject(context.gameObjectId))
            {
                go->SetLocalTransform(context.newTransform);
            }
            else
            {
                LOG_WARN << "GameObject with Id" << context.gameObjectId << " no longer exist!";
            }
        }
    }

    void undo() override
    {
        for (auto& context : contexts)
        {
            if (auto go = scene.GetGameObject(context.gameObjectId))
            {
                go->SetLocalTransform(context.oldTransform);
            }
            else
            {
                LOG_WARN << "GameObject with Id" << context.gameObjectId << " no longer exist!";
            }
        }
    }

private:
    GameEngine::Scene& scene;
    std::vector<Context> contexts;
};