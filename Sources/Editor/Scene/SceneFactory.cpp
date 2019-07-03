#include "GuiEditScene.h"
#include "SceneFactory.h"
#include "EditorScene.h"
#include "ChooseEditor.h"

namespace Editor
{
SceneFactory::SceneFactory()
{
    AddScene("ChooseEditor", []() { return std::make_unique<ChooseEditor>(); });
    AddScene("GuiEditScene", []() { return std::make_unique<GuiEditScene>(); });
    AddScene("EditorScene", []() { return std::make_unique<EditorScene>(); });
}
}  // namespace Editor
