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
        for (auto& childNode : childrenNode->getChildren())
        {
            if (childNode)
            {
                auto name  = childNode->getAttributeValue(CSTR_NAME);
                auto child = scene.CreateGameObject(name);
                DEBUG_LOG(name);
                Read(scene, *childNode, *child);
                DEBUG_LOG("Read done, adding");
                gameObject.AddChild(std::move(child));
                DEBUG_LOG("Add done");
            }
            else
            {
                ERROR_LOG("Somthing goes wrong. Child is empty");
            }
        }
    }
}

GameObject* loadPrefab(Scene& scene, const File& file, const std::string& name)
{
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsoultePath()))
    {
        ERROR_LOG("Prefab read error");
        return nullptr;
    }

    currentReadingScene = &scene;
    DEBUG_LOG("Name : " + name);

    auto gameObject = scene.CreateGameObject(name);
    Read(scene, *xmlReader.Get(CSTR_PREFAB), *gameObject);

    auto result = gameObject.get();
    scene.AddGameObject(std::move(gameObject));
    return result;
}
GameObject* createGameObjectFromPrefabNodeInRootNode(Scene& scene, const TreeNode& node, const std::string& name)
{
    auto gameObject = createGameObjectFromPrefabNode(scene, node, name);
    auto result = gameObject.get();
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
    for (const auto& child : node.getChild(CSTR_GAMEOBJECTS)->getChildren())
    {
        auto gameObject = createGameObject(*child, scene);
        Read(scene, *child, *gameObject);
        scene.AddGameObject(std::move(gameObject));
    }
}

void loadScene(Scene& scene, const File& file)
{
    Utils::XmlReader xmlReader;
    if (not xmlReader.Read(file.GetAbsoultePath()))
        return;

    currentReadingScene = &scene;
    readNode(*xmlReader.Get(CSTR_SCENE), scene);
}
}  // namespace SceneReader
}  // namespace GameEngine
