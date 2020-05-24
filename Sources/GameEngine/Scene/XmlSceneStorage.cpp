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
        SceneReader::readNode(*rootNode_, scene_);
    }
    else
    {
        ERROR_LOG("First store scene before read");
    }
}
void XmlSceneStorage::saveToFile(const File& file)
{
    if (not rootNode_)
    {
        store();
    }

    Utils::CreateBackupFile(file.GetAbsoultePath());
    Utils::Xml::Write(file.GetAbsoultePath(), *rootNode_);
}
void XmlSceneStorage::readFromFile(const File& file)
{
    SceneReader::loadScene(scene_, file);
}
GameObject* XmlSceneStorage::loadPrefab(const File& file, const std::string& name)
{
    return SceneReader::loadPrefab(scene_, file, name);
}
}  // namespace GameEngine
