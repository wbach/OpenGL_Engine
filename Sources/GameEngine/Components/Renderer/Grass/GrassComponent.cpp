#include "GrassComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
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

GrassRendererComponent& GrassRendererComponent::SetPositions(const std::vector<float>& positions)
{
    positions_ = positions;
    return *this;
}

GrassRendererComponent& GrassRendererComponent::SetTexture(const std::string & filename)
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
Mesh GrassRendererComponent::CreateGrassMesh(const Material &material) const
{
    Mesh mesh(GraphicsApi::RenderType::POINTS, componentContext_.resourceManager_.GetGraphicsApi(), material);
    mesh.GetMeshDataRef().positions_ = positions_;
    return mesh;
}
void GrassRendererComponent::CreateGrassModel()
{
    auto model = new Model();
    auto material = CreateGrassMaterial();
    auto mesh = CreateGrassMesh(material);
    model->AddMesh(mesh);
    model_.Add(model, LevelOfDetail::L1);
    componentContext_.resourceManager_.AddModel(model);
}
Material GrassRendererComponent::CreateGrassMaterial() const
{
    Material grass_material;
    grass_material.diffuseTexture = componentContext_.resourceManager_.GetTextureLaoder().LoadTexture(textureFile_);
    return grass_material;
}

}  // namespace Components
}  // namespace GameEngine
