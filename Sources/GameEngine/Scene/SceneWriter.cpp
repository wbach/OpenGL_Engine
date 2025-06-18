#include "SceneWriter.h"

#include <Utils.h>
#include <Utils/GLM/GLMUtils.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Scene/Scene.hpp"
#include "SceneDef.h"
#include "Utils/Variant.h"

using namespace Utils;

namespace GameEngine
{
void Create(TreeNode& node, const vec3& v)
{
    node.attributes_[CSTR_X] = std::to_string(v.x);
    node.attributes_[CSTR_Y] = std::to_string(v.y);
    node.attributes_[CSTR_Z] = std::to_string(v.z);
}
void Create(TreeNode& node, const common::Transform& transform)
{
    Create(node.addChild(CSTR_POSITION), transform.GetPosition());
    Create(node.addChild(CSTR_ROTATION), transform.GetRotation().GetEulerDegrees().value);
    Create(node.addChild(CSTR_SCALE), transform.GetScale());
}
void Create(TreeNode& node, const std::vector<std::unique_ptr<Components::IComponent>>& components)
{
    node.attributes_[CSTR_COUNT] = std::to_string(components.size());
    for (const auto& component : components)
    {
        component->write(node.addChild(CSTR_COMPONENT));
    }
}
void CreatePrefabNode(TreeNode& node, const GameObject& gameObject)
{
    node.addChild(CSTR_FILE_NAME, gameObject.getPrefabricatedFile().GetDataRelativeDir());
}
void Create(TreeNode& node, const GameObject& gameObject)
{
    node.attributes_.insert({CSTR_NAME, gameObject.GetName()});
    node.attributes_.insert({CSTR_ID, std::to_string(gameObject.GetId())});
    Create(node.addChild(CSTR_TRANSFORM), gameObject.GetTransform());
    Create(node.addChild(CSTR_COMPONENTS), gameObject.GetComponents());

    if (not gameObject.GetChildren().empty())
    {
        auto& childrenNode = node.addChild(CSTR_CHILDREN);

        for (const auto& gameObject : gameObject.GetChildren())
        {
            if (not gameObject->isPrefabricated())
            {
                Create(childrenNode.addChild(CSTR_GAMEOBJECT), *gameObject);
            }
            else
            {
                CreatePrefabNode(childrenNode.addChild(CSTR_PREFAB), *gameObject);
            }
        }
    }
}

void Create(TreeNode& node, const GameObjects& gameObjects)
{
    node.attributes_[CSTR_COUNT] = std::to_string(gameObjects.size());

    for (const auto& gameObject : gameObjects)
    {
        if (not gameObject->isPrefabricated())
        {
            Create(node.addChild(CSTR_GAMEOBJECT), *gameObject);
        }
        else
        {
            CreatePrefabNode(node.addChild(CSTR_PREFAB), *gameObject);
        }
    }
}

void CreatePrefabs(TreeNode& node, const GameObjects& gameObjects)
{
    auto objectCount =
        std::count_if(gameObjects.begin(), gameObjects.end(), [](const auto& obj) { return obj->isPrefabricated(); });

    node.attributes_[CSTR_COUNT] = std::to_string(objectCount);

    for (const auto& gameObject : gameObjects)
    {
        if (gameObject->isPrefabricated())
        {
            Create(node.addChild(CSTR_GAMEOBJECT), *gameObject);
        }
    }
}

TreeNode createTree(const Scene& input)
{
    TreeNode scene(CSTR_SCENE);
    scene.attributes_[CSTR_NAME] = input.GetName();
    Create(scene.addChild(CSTR_GAMEOBJECTS), input.GetGameObjects());
    return scene;
}
TreeNode createPrefab(const GameObject& gameObject)
{
    TreeNode rootNode(CSTR_PREFAB);
    Create(rootNode, gameObject);
    return rootNode;
}
}  // namespace GameEngine
