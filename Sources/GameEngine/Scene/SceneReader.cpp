#include "SceneReader.h"

#include <Utils/TreeNode.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Prefab.h"
#include "GameEngine/Resources/File.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneDef.h"
#include "Utils.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"

using namespace Utils;

namespace GameEngine
{

SceneReader::SceneReader(Scene& scene)
    : scene(scene)
{
}

GameObject* SceneReader::loadPrefab(const File& file, const std::string& gameObjectName)
{
    auto gameObject = scene.CreatePrefabGameObject(gameObjectName);
    ReadPrefab(file, *gameObject);
    LOG_DEBUG << "Prefab components size=" << gameObject->GetComponents().size();
    auto result = gameObject.get();
    scene.AddGameObject(std::move(gameObject));
    return result;
}
GameObject* SceneReader::createGameObjectFromPrefabNodeInRootNode(const TreeNode& node, const std::string& name)
{
    auto gameObject = createGameObjectFromPrefabNode(node, name);
    auto result     = gameObject.get();
    scene.AddGameObject(std::move(gameObject));
    return result;
}
std::unique_ptr<GameObject> SceneReader::createGameObjectFromPrefabNode(const TreeNode& node, const std::string& name)
{
    auto gameObject = scene.CreateGameObject(name);
    Read(node, *gameObject);
    return gameObject;
}
void SceneReader::readNode(const TreeNode& node)
{
    try
    {
        auto timeStr = node.getAttributeValue(CSTR_TIME);
        if (not timeStr.empty())
        {
            scene.GetDayNightCycle().SetTime(std::stof(timeStr));
        }
        else
        {
            LOG_DEBUG << "Time not set.";
        }
    }
    catch (...)
    {
        LOG_WARN << "Time read error";
    }

    if (auto maybeGameObjectsNode = node.getChild(CSTR_GAMEOBJECTS))
    {
        for (const auto& gameObjectNode : maybeGameObjectsNode->getChildren())
        {
            if (not gameObjectNode)
            {
                LOG_WARN << "Null child !";
                continue;
            }
            if (gameObjectNode->name() == CSTR_GAMEOBJECT)
            {
                LOG_DEBUG << "read gameobject";
                auto gameObject = createGameObject(*gameObjectNode);
                auto ptr        = gameObject.get();
                Read(*gameObjectNode, *ptr);
                scene.AddGameObject(std::move(gameObject));
            }
            else if (gameObjectNode->name() == CSTR_PREFAB)
            {
                LOG_DEBUG << "read prefab";
                auto gameObject = createPrefabGameObject(*gameObjectNode);
                scene.AddGameObject(std::move(gameObject));
            }
        }
    }
}

std::optional<TreeNode> SceneReader::loadScene(const File& file)
{
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsolutePath().string()))
        return std::nullopt;

    auto sceneNode = xmlReader.Get(CSTR_SCENE);
    if (sceneNode)
    {
        readNode(*sceneNode);
        return TreeNode(*sceneNode);
    }

    return std::nullopt;
}

std::unique_ptr<GameObject> SceneReader::createGameObject(const TreeNode& node)
{
    std::string name;
    if (node.attributes_.count(CSTR_NAME))
    {
        name = node.attributes_.at(CSTR_NAME);
    }
    else
    {
        static int nonameid = 0;
        name                = std::string{"NoName_" + std::to_string(nonameid++)};
    }

    if (node.attributes_.count(CSTR_ID))
    {
        uint32 id = std::stoi(node.attributes_.at(CSTR_ID));
        return scene.CreateGameObject(name, id);
    }

    return scene.CreateGameObject(name);
}

std::unique_ptr<Prefab> SceneReader::createPrefabGameObject(const TreeNode& node)
{
    std::string name;
    if (node.attributes_.count(CSTR_NAME))
    {
        name = node.attributes_.at(CSTR_NAME);
    }
    else
    {
        static int nonameid = 0;
        name                = std::string{"NoName_" + std::to_string(nonameid++)};
    }

    std::optional<uint32> maybeId;
    if (node.attributes_.count(CSTR_ID))
    {
        maybeId = std::stoi(node.attributes_.at(CSTR_ID));
    }

    auto gameObject = scene.CreatePrefabGameObject(name, maybeId);

    Read(node, *gameObject);

    if (auto maybeFileNameNode = node.getChild(CSTR_FILE_NAME))
    {
        ReadPrefab(maybeFileNameNode->value_, *gameObject);
    }

    return gameObject;
}

void SceneReader::Read(const TreeNode& node, common::Transform& tranfsorm)
{
    vec3 position(0), rotation(0), scale(1);
    ::Read(node.getChild(CSTR_POSITION), position);
    ::Read(node.getChild(CSTR_ROTATION), rotation);
    ::Read(node.getChild(CSTR_SCALE), scale);
    tranfsorm.SetPositionAndRotationAndScale(position, DegreesVec3(rotation), scale);
}

template <typename T>
void SceneReader::AddComponent(const TreeNode& node, GameObject& gameObject)
{
    auto& comp = gameObject.AddComponent<T>();
    Read(node, comp);
}

void SceneReader::Read(const TreeNode& node, GameObject& gameObject)
{
    auto transformNode = node.getChild(CSTR_TRANSFORM);
    if (transformNode)
    {
        Read(*transformNode, gameObject.localTransform_);
    }

    auto componentsNode = node.getChild(CSTR_COMPONENTS);
    if (componentsNode)
    {
        for (const auto& component : componentsNode->getChildren())
        {
            gameObject.AddComponent(*component);
        }
    }

    auto childrenNode = node.getChild(CSTR_CHILDREN);
    if (childrenNode)
    {
        for (auto& gameObjectNode : childrenNode->getChildren())
        {
            if (gameObjectNode->name() == CSTR_GAMEOBJECT)
            {
                auto name  = gameObjectNode->getAttributeValue(CSTR_NAME);
                auto child = scene.CreateGameObject(name);
                Read(*gameObjectNode, *child);
                gameObject.AddChild(std::move(child));
            }
            else if (gameObjectNode->name() == CSTR_PREFAB)
            {
                auto child = createPrefabGameObject(*gameObjectNode);
                gameObject.AddChild(std::move(child));
            }
        }
    }
}

void SceneReader::ReadPrefab(const File& file, Prefab& prefabGameObject)
{
    prefabGameObject.setFile(file);

    LOG_DEBUG << "LoadPrefabFile : " << file;
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsolutePath().string()))
    {
        LOG_ERROR << "Prefab read error file: " << file;
        return;
    }

    auto maybePrefabNode = xmlReader.Get(CSTR_PREFAB);
    if (not maybePrefabNode)
    {
        return;
    }

    auto name = maybePrefabNode->getAttributeValue(CSTR_NAME);
    if (prefabGameObject.GetName().empty())
    {
        prefabGameObject.SetName(name);
    }

    auto gameObject = scene.CreateGameObject(name);
    Read(*maybePrefabNode, *gameObject);
    prefabGameObject.AddChild(std::move(gameObject));
}

}  // namespace GameEngine
