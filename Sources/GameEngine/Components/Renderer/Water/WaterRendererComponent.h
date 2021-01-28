#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
namespace Components
{
class WaterRendererComponent : public BaseComponent
{
public:
    WaterRendererComponent(ComponentContext&, GameObject&);
    ~WaterRendererComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;

    float moveFactor() const;
    float increaseAndGetMoveFactor(float deltaTime);

    const vec4& GetWaterColor() const;
    float GetWaveSpeed() const;
    const Texture* GetDudvTexture() const;
    const Texture* GetNormalTexture() const;

    void SetWaterColor(const vec4&);
    WaterRendererComponent& SetWaveSpeed(float);
    WaterRendererComponent& LoadTextures(const File& dudv, const File& normal);

    GraphicsApi::ID getPerObjectUpdateBufferId() const;

private:
    void OnAwake();
    void UnSubscribe();
    void updatePerObjectUpdateBuffer();

private:
    void DeleteTextures();

private:
    std::unique_ptr<BufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::optional<uint32> onTransformChangeSubscribtion_;

    float moveFactor_;
    vec4 waterColor_;
    float waveSpeed_;

    Texture* normalMap_;
    Texture* dudvMap_;

    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
