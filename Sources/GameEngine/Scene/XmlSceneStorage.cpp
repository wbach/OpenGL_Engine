#include "XmlSceneStorage.h"

#include <Utils/Logger/Log.h>
#include <Utils/XML/XmlWriter.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Scene/Scene.hpp"
#include "SceneReader.h"
#include "SceneWriter.h"

namespace GameEngine
{
XmlSceneStorage::XmlSceneStorage(Scene& scene)
    : scene_(scene)
{
}
XmlSceneStorage::~XmlSceneStorage()
{
}
void XmlSceneStorage::store()
{
    DEBUG_LOG("store");
    rootNode_ = createTree(scene_);
}
void XmlSceneStorage::restore()
{
    if (rootNode_)
    {
        DEBUG_LOG("restore");
        scene_.ClearGameObjects();
        DEBUG_LOG("clear objects completed");
        SceneReader::readNode(*rootNode_, scene_);
        DEBUG_LOG("restore completed");
    }
    else
    {
        ERROR_LOG("First store scene before read");
    }
}
void XmlSceneStorage::saveToFile(const File& file)
{
    if (not scene_.isStarted())
    {
        store();
    }

    if (rootNode_)
    {
        Utils::CreateBackupFile(file.GetAbsoultePath());
        Utils::Xml::Write(file.GetAbsoultePath(), *rootNode_);
    }
    else
    {
        ERROR_LOG("Save scene to file wihout storing scene.");
    }
}
void XmlSceneStorage::readFromFile(const File& file)
{
    SceneReader::loadScene(scene_, file);
}
GameObject* XmlSceneStorage::loadPrefab(const File& file, const std::string& name)
{
    return SceneReader::loadPrefab(scene_, file, name);
}
void XmlSceneStorage::createPrefab(const File& file, const GameObject& gameObject)
{
    auto prefabRootNode = GameEngine::createPrefab(gameObject);
    Utils::Xml::Write(file.GetAbsoultePath(), prefabRootNode);
}
GameObject* XmlSceneStorage::clone(const GameObject& gameObject)
{
    auto prefabRootNode = GameEngine::createPrefab(gameObject);
    auto clonedGameObject =
        SceneReader::createGameObjectFromPrefabNode(scene_, prefabRootNode, "clone_of_" + gameObject.GetName());
    auto result = clonedGameObject.get();
    if (auto parent = gameObject.GetParent())
    {
        parent->AddChild(std::move(clonedGameObject));
    }
    else
    {
        scene_.AddGameObject(std::move(clonedGameObject));
    }
    return result;
}
}  // namespace GameEngine
