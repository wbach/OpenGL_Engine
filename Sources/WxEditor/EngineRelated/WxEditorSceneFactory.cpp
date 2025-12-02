#include "WxEditorSceneFactory.h"

#include "EmptyScene.h"
#include "NewScene.h"
#include "WxScenesDef.h"

namespace WxEditor
{
WxEditorSceneFactory::WxEditorSceneFactory()
{
    GameEngine::SceneFactoryBase::AddScene(NEW_SCENE, []() { return std::make_unique<NewScene>(); });
    GameEngine::SceneFactoryBase::AddScene(EMPTY_SCENE, []() { return std::make_unique<EmptyScene>(); });
}
void WxEditorSceneFactory::AddScene(const std::string& sceneName, const GameEngine::File& file)
{
    GameEngine::SceneFactoryBase::AddScene(sceneName, file);
}
}  // namespace WxEditor
