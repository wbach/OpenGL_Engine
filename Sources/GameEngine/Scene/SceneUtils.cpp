#include "SceneUtils.h"

#include <Utils/XML/XmlReader.h>
#include <Utils/XML/XmlWriter.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#include "Scene.hpp"
#include "SceneDef.h"
#include "SceneReader.h"
#include "SceneWriter.h"

typedef GameEngine::Scene* (*CreateSceneFromLib)();

namespace GameEngine
{
void saveSceneToFile(Scene& scene)
{
    const auto& file = scene.GetFile();
    if (not file.empty())
    {
        saveSceneToFile(createTree(scene), file);
    }
    else
    {
        auto sceneName = scene.GetName();
        File file("Scenes/" + sceneName + "/" + sceneName + ".xml");
        saveSceneToFile(scene, file);
    }
}

void saveSceneToFile(Scene& scene, const File& file)
{
    scene.GetFile() = file;
    saveSceneToFile(createTree(scene), file);
}

void saveSceneToFile(const TreeNode& rootNode, const File& file)
{
    auto path = file.GetAbsolutePathWithDifferentExtension("xml");
    Utils::CreateBackupFile(path);
    Utils::Xml::Write(path, rootNode);
}
void createAndSavePrefab(const File& file, const GameObject& gameObject)
{
    auto prefabRootNode = GameEngine::createPrefab(gameObject);
    Utils::Xml::Write(file.GetAbsolutePathWithDifferentExtension("prefab"), prefabRootNode);
}
GameObject* cloneGameObject(Scene& scene, const GameObject& gameObject)
{
    auto prefabRootNode = GameEngine::createPrefab(gameObject);
    auto clonedGameObject =
        SceneReader::createGameObjectFromPrefabNode(scene, prefabRootNode, "clone_of_" + gameObject.GetName());
    auto result = clonedGameObject.get();
    if (auto parent = gameObject.GetParent())
    {
        parent->AddChild(std::move(clonedGameObject));
    }
    else
    {
        scene.AddGameObject(std::move(clonedGameObject));
    }
    return result;
}
}  // namespace GameEngine
