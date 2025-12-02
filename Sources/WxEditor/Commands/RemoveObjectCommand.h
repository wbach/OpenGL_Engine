#pragma once

#include <Common/Transform.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Scene/SceneReader.h>
#include <GameEngine/Scene/SceneWriter.h>
#include <Logger/Log.h>
#include <Types.h>

#include <GameEngine/Scene/Scene.hpp>
#include <unordered_map>

#include "Command.h"
#include "TreeNode.h"

class RemoveObjectCommand : public Command
{
public:
    RemoveObjectCommand(GameEngine::Scene& scene, const GameEngine::GameObject& go)
        : scene(scene)
        , gameObjectId{go.GetId()}
        , gameObjectNode("tmp")
    {
        GameEngine::Create(gameObjectNode, go);
    }

    void redo() override
    {
        if (auto go = scene.GetGameObject(gameObjectId))
        {
            scene.RemoveGameObject(*go);
        }
        else
        {
            LOG_WARN << "GameObject with Id" << gameObjectId << " no longer exist!";
        }
    }

    void undo() override
    {
        GameEngine::SceneReader reader{scene};
        auto restoredGameObject = scene.CreateGameObject(gameObjectNode.attributes_.at("name"), gameObjectId);
        reader.Read(gameObjectNode, *restoredGameObject);
        scene.AddGameObject(std::move(restoredGameObject));
    }

private:
    GameEngine::Scene& scene;
    IdType gameObjectId;
    TreeNode gameObjectNode;
};