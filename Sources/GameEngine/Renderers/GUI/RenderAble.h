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

    virtual std::optional<uint32> getTextureId() const;

    void setColor(const Color&);
    const Color& getColor() const;
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
    Color color_;
    mat4 transformMatrix_;
    int inactivityReleaseTime{5000};
    std::optional<Timepoint> releaseTimerStart_;
    std::optional<IdType> changeSubId;
};
}  // namespace GUI
}  // namespace GameEngine
