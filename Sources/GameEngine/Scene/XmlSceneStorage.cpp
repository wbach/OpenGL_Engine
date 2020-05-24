#include "XmlSceneStorage.h"

namespace GameEngine
{
XmlSceneStorage::XmlSceneStorage(Scene &scene)
    : scene_(scene)
    , rootNode_("scene")
{
}
XmlSceneStorage::~XmlSceneStorage()
{
}
void XmlSceneStorage::store()
{
}
void XmlSceneStorage::saveToFile(const File &)
{
}
void XmlSceneStorage::readFromFile(const File &)
{
}
}  // namespace GameEngine
