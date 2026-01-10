#pragma once
#include <unordered_map>
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/ShaderBufferObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace common
{
class Transform;
}

namespace GameEngine
{
class Texture;

namespace Components
{
class WaterRendererComponent : public BaseComponent
{
public:
    float onPlaneWaveSpeed{1.f};
    float waveSpeed{1.f};
    float waveAmplitude{.5f};
    float waveFrequency{0.5f};
    float tiledValue{1.f};
    float maxDepthVisibility{15.f};
    float depthBlendScale{2.f};
    vec4 waterColor;
    File dudvMap;
    File normalMap;
    uint32 meshResolution;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_COLOR_RGBA(waterColor)
        FIELD_TEXTURE(dudvMap)
        FIELD_TEXTURE(normalMap)
        FIELD_FLOAT(tiledValue)
        FIELD_FLOAT(waveAmplitude)
        FIELD_FLOAT(waveFrequency)
        FIELD_FLOAT(waveSpeed)
        FIELD_FLOAT(onPlaneWaveSpeed)
        FIELD_FLOAT(maxDepthVisibility)
        FIELD_FLOAT(depthBlendScale)
        FIELD_UINT(meshResolution)
    END_FIELDS()
    // clang-format on
public:
    WaterRendererComponent(ComponentContext&, GameObject&);
    ~WaterRendererComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    float waveMoveFactor() const;
    float moveFactor() const;
    float increaseFactors(float);

    const vec4& GetWaterColor() const;
    float GetWaveSpeed() const;
    const Texture* GetDudvTexture() const;
    const Texture* GetNormalTexture() const;

    void SetWaterColor(const vec4&);
    WaterRendererComponent& SetWaveSpeed(float);
    WaterRendererComponent& LoadTextures(const File& dudv, const File& normal);

    GraphicsApi::ID getPerObjectUpdateBufferId() const;

    Model* GetModel();
    const BoundingBox& getMeshBoundingBox(const Mesh&) const;
    const BoundingBox& getModelBoundingBox() const;

private:
    void OnAwake();
    void UnSubscribe();
    void updatePerObjectUpdateBuffer(const common::Transform&);
    void createModel();
    void createBoundingBoxes();

private:
    void DeleteTextures();

private:
    std::unique_ptr<ShaderBufferObject<PerObjectUpdate>> perObjectUpdateBuffer_;
    std::optional<uint32> onTransformChangeSubscribtion_;

    float moveFactor_;
    float waveMove_;
    Texture* normalMap_;
    Texture* dudvMap_;

    bool isSubscribed_;

    ModelWrapper modelWrapper_;
    BoundingBox modelBoundingBox_;
    std::unordered_map<const Mesh*, BoundingBox> meshBoundingBoxes_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
