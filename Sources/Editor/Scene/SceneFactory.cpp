#include "GuiEditScene.h"
#include "SceneFactory.h"

namespace Editor
{
SceneFactory::SceneFactory()
{
    AddScene("GuiEditScene", []() { return std::make_unique<GuiEditScene>(); });
}
}  // namespace Editor
