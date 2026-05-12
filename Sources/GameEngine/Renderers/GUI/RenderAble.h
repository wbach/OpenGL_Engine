#pragma once
#include <optional>

#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "Types.h"

namespace GameEngine
{
class IResourceManager;

namespace GUI
{
class Renderer;

class ENGINE_API RenderAble : public Element
{
public:
    RenderAble(IResourceManager&, Renderer&);
    ~RenderAble() override;

    RenderAble(const RenderAble&);

    virtual std::optional<uint32> getTextureId() const;

    void setTextureColor(const Color&);
    void setBackgroundColor(const Color&);
    const Color& getTextureColor() const;
    const Color& getBackgroundColor() const;
    const GeneralTexture* getTexture() const;

    const mat4& getTransformMatrix() const;

    void onRender();
    void onSkipRender();
    void inactivityRelease(int);
    int inactivityRelease() const;

protected:
    virtual void updateTexture();
    virtual void updateTransformMatrix();
    void deleteTexture();

protected:
    IResourceManager& resourceManager_;
    Renderer& renderer_;
    GeneralTexture* texture_;
    Color textureColor_;
    Color backgroundColor_;
    mat4 transformMatrix_;
    int inactivityReleaseTime{5000};
    std::optional<Timepoint> releaseTimerStart_;
};
}  // namespace GUI
}  // namespace GameEngine
