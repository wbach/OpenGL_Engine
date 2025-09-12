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
    /* LOG TO FIX*/  LOG_ERROR << ("store");
    rootNode_ = createTree(scene_);
}
void XmlSceneStorage::restore()
{
    if (rootNode_)
    {
        /* LOG TO FIX*/  LOG_ERROR << ("restore");
        scene_.ClearGameObjects();
        /* LOG TO FIX*/  LOG_ERROR << ("clear objects completed");
        SceneReader::readNode(*rootNode_, scene_);
        /* LOG TO FIX*/  LOG_ERROR << ("restore completed");
    }
    else
    {
        /* LOG TO FIX*/  LOG_ERROR << ("First store scene before read");
    }
}
}  // namespace GameEngine
