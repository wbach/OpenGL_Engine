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
    , maybeModifyObjectsTreeNode_{std::nullopt}
{
}
XmlSceneStorage::~XmlSceneStorage()
{
}
void XmlSceneStorage::store()
{
    DEBUG_LOG("store");
    rootNode_ = createTree(scene_);
    if (maybeModifyObjectsTreeNode_)
    {
         auto& newModifyNode = rootNode_->addChild(CSTR_MODIFY_OBJECTS);
         newModifyNode       = *maybeModifyObjectsTreeNode_;
    }
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
}  // namespace GameEngine
