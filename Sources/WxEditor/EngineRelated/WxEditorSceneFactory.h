#pragma once
#include <GameEngine/Scene/SceneFactoryBase.h>

#include <string>

namespace WxEditor
{
class WxEditorSceneFactory : public GameEngine::SceneFactoryBase
{
public:
    WxEditorSceneFactory();
    void AddScene(const std::string&, const GameEngine::File&);
};
}  // namespace WxEditor
