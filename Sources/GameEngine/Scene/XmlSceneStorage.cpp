#include "XmlSceneStorage.h"

#include <Utils/Logger/Log.h>
#include <Utils/XML/XmlWriter.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Scene/Scene.hpp"
#include "SceneDef.h"
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
    LOG_DEBUG << "store";
    rootNode_ = createTree(scene_);
}
void XmlSceneStorage::restore()
{
    if (rootNode_)
    {
        LOG_DEBUG << "restore";
        scene_.ClearGameObjects();
        LOG_DEBUG << "clear objects completed";
        SceneReader(scene_).readNode(*rootNode_);
        LOG_DEBUG << "restore completed";
    }
    else
    {
        LOG_DEBUG << "First store scene before read";
    }
}
}  // namespace GameEngine
