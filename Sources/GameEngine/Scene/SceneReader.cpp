#include "SceneReader.h"

#include "GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "Scene.hpp"
#include "SceneDef.h"
#include "Utils.h"
#include "Utils/XML/XMLUtils.h"
#include "Utils/XML/XmlReader.h"

using namespace Utils;

namespace GameEngine
{
namespace SceneReader
{
namespace
{
Scene* currentReadingScene = nullptr;
}

void Read(const TreeNode& node, common::Transform& tranfsorm)
{
    vec3 position(0), rotation(0), scale(1);
    Read(node.getChild(CSTR_POSITION), position);
    Read(node.getChild(CSTR_ROTATION), rotation);
    Read(node.getChild(CSTR_SCALE), scale);
    tranfsorm.SetPositionAndRotationAndScale(position, DegreesVec3(rotation), scale);
}

template <typename T>
void AddComponent(const TreeNode& node, GameObject& gameObject)
{
    auto& comp = gameObject.AddComponent<T>();
    Read(node, comp);
}

void ModifyGameObject(Scene& scene, const TreeNode& node, GameObject& gameObject)
{
    auto transformNode = node.getChild(CSTR_TRANSFORM);
    if (transformNode)
    {
        Read(*transformNode, gameObject.GetTransform());
    }

    auto componentsNode = node.getChild(CSTR_COMPONENTS);
    if (componentsNode)
    {
        for (const auto& component : componentsNode->getChildren())
        {
            DEBUG_LOG("TO DO: AddOrModifyComponent: " + component->name());
            // gameObject.InitComponent(*component);
        }
    }

    auto childrenNode = node.getChild(CSTR_CHILDREN);
    if (childrenNode)
    {
        DEBUG_LOG("TO DO: AddOrModifyChildren: " + childrenNode->name());
        // for (auto& childNode : childrenNode->getChildren())
        //{
        //     if (childNode)
        //     {
        //         auto name  = childNode->getAttributeValue(CSTR_NAME);
        //         auto child = scene.CreateGameObject(name);
        //         Read(scene, *childNode, *child);
        //         gameObject.AddChild(std::move(child));
        //     }
        //     else
        //     {
        //         ERROR_LOG("Somthing goes wrong. Child is empty");
        //     }
        // }
    }
}

void ReadPrefab(const TreeNode& node, Scene& scene)
{
    if (auto maybeFileNameNode = node.getChild(CSTR_FILE_NAME))
    {
        loadPrefab(scene, maybeFileNameNode->value_);
    }
}


void Read(Scene& scene, const TreeNode& node, GameObject& gameObject)
{
    auto transformNode = node.getChild(CSTR_TRANSFORM);
    if (transformNode)
    {
        Read(*transformNode, gameObject.GetTransform());
    }

    auto componentsNode = node.getChild(CSTR_COMPONENTS);
    if (componentsNode)
    {
        for (const auto& component : componentsNode->getChildren())
        {
            gameObject.InitComponent(*component);
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
                Read(scene, *gameObjectNode, *child);
                gameObject.AddChild(std::move(child));
            }
            else if (gameObjectNode->name() == CSTR_PREFAB)
            {
                ReadPrefab(*gameObjectNode, scene);
            }
        }
    }
}

GameObject* ReadPrefab(const File& file, Scene& scene, const TreeNode& node, const std::string& gameObjectName)
{
    auto prefabName = gameObjectName;

    if (prefabName.empty())
    {
        prefabName = node.getAttributeValue("name");
    }

    if (prefabName.empty())
    {
        prefabName = "Prefab_" + file.GetBaseName();
    }

    auto gameObject = scene.CreateGameObject(prefabName);
    gameObject->markAsPrefabricated(file);
    Read(scene, node, *gameObject);

    auto result = gameObject.get();
    scene.AddGameObject(std::move(gameObject));
    return result;
}

GameObject* loadPrefab(Scene& scene, const File& file, const std::string& gameObjectName)
{
    DEBUG_LOG("LoadPrefab : " + file.GetAbsoultePath());
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsoultePath()))
    {
        ERROR_LOG("Prefab read error file: " + file.GetAbsoultePath());
        return nullptr;
    }

    currentReadingScene = &scene;

    auto maybePrefabNode = xmlReader.Get(CSTR_PREFAB);
    if (not maybePrefabNode)
    {
        return nullptr;
    }
    return ReadPrefab(file, scene, *maybePrefabNode, gameObjectName);
}
GameObject* createGameObjectFromPrefabNodeInRootNode(Scene& scene, const TreeNode& node, const std::string& name)
{
    auto gameObject = createGameObjectFromPrefabNode(scene, node, name);
    auto result     = gameObject.get();
    scene.AddGameObject(std::move(gameObject));
    return result;
}
std::unique_ptr<GameObject> createGameObjectFromPrefabNode(Scene& scene, const TreeNode& node, const std::string& name)
{
    auto gameObject = scene.CreateGameObject(name);
    Read(scene, node, *gameObject);
    return gameObject;
}
std::unique_ptr<GameObject> createGameObject(const TreeNode& node, Scene& scene)
{
    if (node.attributes_.count(CSTR_ID))
    {
        uint32 id = std::stoi(node.attributes_.at(CSTR_ID));
        return scene.CreateGameObject(node.attributes_.at(CSTR_NAME), id);
    }

    return scene.CreateGameObject(node.attributes_.at(CSTR_NAME));
}

void readNode(const TreeNode& node, Scene& scene)
{
    if (auto maybeGameObjectsNode = node.getChild(CSTR_GAMEOBJECTS))
    {
        for (const auto& gameObjectNode : maybeGameObjectsNode->getChildren())
        {
            if (gameObjectNode->name() == CSTR_GAMEOBJECT)
            {
                DEBUG_LOG("read gameobject");
                auto gameObject = createGameObject(*gameObjectNode, scene);
                Read(scene, *gameObjectNode, *gameObject);
                scene.AddGameObject(std::move(gameObject));
            }
            else if (gameObjectNode->name() == CSTR_PREFAB)
            {
                DEBUG_LOG("read prefab");
                ReadPrefab(*gameObjectNode, scene);
            }
        }
    }
    if (auto maybeModifyObjectsNode = node.getChild(CSTR_MODIFY_OBJECTS))
    {
        for (const auto& gameObjectNode : maybeModifyObjectsNode->getChildren())
        {
            auto nameAttributeIter = gameObjectNode->attributes_.find("name");
            if (nameAttributeIter != gameObjectNode->attributes_.end())
            {
                auto goName = nameAttributeIter->second;
                if (not goName.empty())
                {
                    if (auto maybeGameObject = scene.GetGameObject(goName))
                    {
                        ModifyGameObject(scene, *gameObjectNode, *maybeGameObject);
                    }
                }
            }
        }
    }
}

TreeNode *loadScene(Scene& scene, const File& file)
{
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsoultePath()))
        return nullptr;

    currentReadingScene = &scene;
    auto sceneNode = xmlReader.Get(CSTR_SCENE);
    if (sceneNode)
    {
        readNode(*sceneNode, scene);
    }
    return sceneNode;
}
}  // namespace SceneReader
}  // namespace GameEngine
