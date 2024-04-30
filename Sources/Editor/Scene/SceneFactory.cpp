#include "SceneFactory.h"
#include "ChooseEditor.h"
#include "Editor/Context.h"
#include "EditorScene.h"
#include "GuiEditScene.h"

namespace Editor
{
SceneFactory::SceneFactory(Context& context)
{
    AddScene("ChooseEditor", []() { return std::make_unique<ChooseEditor>(); });
    AddScene("GuiEditScene", [&context]() { return std::make_unique<GuiEditScene>(context); });
    AddScene("EditorScene", []() { return std::make_unique<EditorScene>(); });
}
}  // namespace Editor
