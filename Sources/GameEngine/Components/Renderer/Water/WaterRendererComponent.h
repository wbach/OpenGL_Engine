#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/File.h"

namespace GameEngine
{
class Texture;

namespace Components
{
class WaterRendererComponent : public BaseComponent
{
public:
    float waveSpeed;
    vec4 waterColor;
    File dudvMap;
    File normalMap;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_TEXTURE(dudvMap)
        FIELD_TEXTURE(normalMap)
        FIELD_FLOAT(waveSpeed)
        FIELD_COLOR_RGBA(waterColor)
    END_FIELDS()
    // clang-format on
public:
    WaterRendererComponent(ComponentContext&, GameObject&);
    ~WaterRendererComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

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
    Texture* normalMap_;
    Texture* dudvMap_;

    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
