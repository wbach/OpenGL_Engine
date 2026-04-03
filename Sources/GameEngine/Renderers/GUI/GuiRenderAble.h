#pragma once
#include <optional>

#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "Types.h"

namespace GameEngine
{
class GUIRenderer;
class IResourceManager;

class ENGINE_API GuiRenderAble : public GuiElement
{
public:
    GuiRenderAble(IResourceManager&, GUIRenderer&, GuiElementTypes);
    ~GuiRenderAble() override;


    virtual std::optional<uint32> GetTextureId() const;

    void SetColor(const Color&);
    const Color& GetColor() const;
    const GeneralTexture* GetTexture() const;

    const mat4& GetTransformMatrix() const;

    void OnRender();
    void OnSkipRender();
    void InactivityRelease(int);
    int InactivityRelease() const;

protected:
    virtual void UpdateTexture();
    virtual void UpdateTransformMatrix();
    void DeleteTexture();

protected:
    IResourceManager& resourceManager_;
    GUIRenderer& guiRenderer_;
    GeneralTexture* texture_;
    Color color_;
    mat4 transformMatrix_;
    int inactivityRelease{5000};
    std::optional<Timepoint> releaseTimerStart_;
    std::optional<IdType> changeSubId;
};
}  // namespace GameEngine
