#pragma once
#include <memory>
#include <vector>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/ShaderStorageVectorBufferObject.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class GrassRendererComponent : public BaseComponent
{
public:
    File textureFile;
    File dataFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_TEXTURE(textureFile)
        FIELD_FILE(dataFile)
    END_FIELDS()
    // clang-format on

public:
    struct Ssbo
    {
        AlignWrapper<vec4> position;
        AlignWrapper<vec4> rotation;
        AlignWrapper<vec4> normal;
        AlignWrapper<vec4> colorAndSizeRandomness;  // xyz - pos, w size
    };
    using SsboType = std::unique_ptr<ShaderStorageVectorBufferObject<Ssbo>>;

    GrassRendererComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    inline const File& getTextureFile() const;
    inline const File& getDataFile() const;

    void AddInstance(const Ssbo&);
    void AddInstances(std::vector<Ssbo>&&);
    void UpdateSsbo(std::vector<Ssbo>&&);
    void RemoveInstances(const std::vector<vec3>&);

    GrassRendererComponent& setTexture(const File&);
    GrassRendererComponent& setDataFile(const File&);

    const GraphicsApi::ID& GetSsboId() const;
    const Material& GetMaterial() const;
    size_t GetCount() const;
    const SsboType& GetSsbo() const;

private:
    void SubscribeToRenderer();
    void UnSubscribe();
    Material CreateMaterial() const;
    void CreateDataFile();
    void CreateSsbo();
    void OnAwake();
    void CleanUpMaterial();

private:
    bool isSubscribed_;

    Material material;
    SsboType ssbo;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};

const File& GrassRendererComponent::getTextureFile() const
{
    return textureFile;
}

const File& GrassRendererComponent::getDataFile() const
{
    return dataFile;
}

}  // namespace Components
}  // namespace GameEngine
