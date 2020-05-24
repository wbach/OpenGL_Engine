#pragma once
#include "ISceneStorage.h"
#include <Utils/XML/XmlNode.h>

namespace GameEngine
{
class Scene;
class XmlSceneStorage : public ISceneStorage
{
public:
    XmlSceneStorage(Scene&);
    ~XmlSceneStorage() override;

    void store() override;
    void saveToFile(const File&) override;
    void readFromFile(const File&) override;

private:
    Scene& scene_;
    Utils::XmlNode rootNode_;
};
}  // namespace GameEngine
