#include "GrassComponent.h"

#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType GrassRendererComponent::type = ComponentsType::Grass;

GrassRendererComponent::GrassRendererComponent(const ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Grass, componentContext, gameObject)
{
}

GrassRendererComponent::~GrassRendererComponent()
{
    UnSubscribe();

    for (auto model : model_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
}

GrassRendererComponent& GrassRendererComponent::SetPositions(const std::vector<float>& positions)
{
    positions_ = positions;
    return *this;
}

GrassRendererComponent& GrassRendererComponent::SetTexture(const std::string& filename)
{
    textureFile_ = filename;
    return *this;
}

void GrassRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&GrassRendererComponent::CreateModelAndSubscribe, this));
}
void GrassRendererComponent::CreateModelAndSubscribe()
{
    CreateGrassModel();
    componentContext_.renderersManager_.Subscribe(&thisObject_);
}
void GrassRendererComponent::UnSubscribe()
{
    componentContext_.renderersManager_.UnSubscribe(&thisObject_);
}
Mesh GrassRendererComponent::CreateGrassMesh(const Material& material) const
{
    Mesh mesh(GraphicsApi::RenderType::POINTS, componentContext_.graphicsApi_, material);
    mesh.GetMeshDataRef().positions_ = positions_;
    return mesh;
}
void GrassRendererComponent::CreateGrassModel()
{
    auto model    = std::make_unique<Model>();
    auto material = CreateGrassMaterial();
    auto mesh     = CreateGrassMesh(material);
    model->AddMesh(mesh);
    model_.Add(model.get(), LevelOfDetail::L1);
    componentContext_.resourceManager_.AddModel(std::move(model));
}
Material GrassRendererComponent::CreateGrassMaterial() const
{
    Material grass_material;
    grass_material.diffuseTexture =
        componentContext_.resourceManager_.GetTextureLoader().LoadTexture(textureFile_, TextureParameters());
    return grass_material;
}

}  // namespace Components
}  // namespace GameEngine
