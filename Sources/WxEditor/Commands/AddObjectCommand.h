#pragma once

#include <Common/Transform.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Scene/SceneReader.h>
#include <GameEngine/Scene/SceneWriter.h>
#include <Logger/Log.h>
#include <Types.h>

#include <GameEngine/Scene/Scene.hpp>

#include "Command.h"
#include "TreeNode.h"

class AddObjectCommand : public Command
{
public:
    AddObjectCommand(GameEngine::Scene& scene, const GameEngine::GameObject& gameObject)
        : scene(scene)
        , gameObjectId{gameObject.GetId()}
        , gameObjectNode("tmpNode")
    {
        LOG_DEBUG << "AddObjectCommand create node";
        GameEngine::Create(gameObjectNode, gameObject);
    }

    void redo() override
    {
        GameEngine::SceneReader reader{scene};
        auto restoredGameObject = scene.CreateGameObject(gameObjectNode.attributes_.at("name"), gameObjectId);
        reader.Read(gameObjectNode, *restoredGameObject);
        scene.AddGameObject(std::move(restoredGameObject));
    }

    void undo() override
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

private:
    GameEngine::Scene& scene;
    IdType gameObjectId;
    TreeNode gameObjectNode;
};