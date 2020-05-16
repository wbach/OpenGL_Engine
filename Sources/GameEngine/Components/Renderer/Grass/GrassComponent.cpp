#include "GrassComponent.h"

#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
ComponentsType GrassRendererComponent::type = ComponentsType::Grass;

GrassRendererComponent::GrassRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(ComponentsType::Grass, componentContext, gameObject)
    , isSubscribed_(false)
{
}

GrassRendererComponent::~GrassRendererComponent()
{
}

void GrassRendererComponent::CleanUp()
{
    UnSubscribe();

    for (auto model : model_.PopModels())
    {
        componentContext_.resourceManager_.ReleaseModel(*model);
    }
}

void GrassRendererComponent::UpdateModel()
{
    auto model = model_.Get(LevelOfDetail::L1);

    if (model and not model->GetMeshes().empty())
    {
        auto& mesh = model->GetMeshes()[0];

        auto& meshData      = mesh.GetMeshDataRef();
        meshData.positions_ = positions_;

        if (mesh.GetGraphicsObjectId())
        {
            componentContext_.gpuResourceLoader_.AddFunctionToCall(
                [& graphicsApi = this->componentContext_.graphicsApi_, &mesh, &meshData]() {
                    graphicsApi.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData, {VertexBufferObjects::POSITION});
                });
        }
    }
    else
    {
        ERROR_LOG("Mesh not set!.");
        return;
    }
}

void GrassRendererComponent::AddNextPosition(const vec3& v)
{
    positions_.push_back(v.x);
    positions_.push_back(v.y);
    positions_.push_back(v.z);
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

void GrassRendererComponent::InitFromParams(const std::unordered_map<std::string, std::string>&)
{
}

std::unordered_map<ParamName, Param> GrassRendererComponent::GetParams() const
{
    return std::unordered_map<ParamName, Param>();
}

void GrassRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&GrassRendererComponent::CreateModelAndSubscribe, this));
}
void GrassRendererComponent::CreateModelAndSubscribe()
{
    CreateGrassModel();

    if (not isSubscribed_)
    {
        componentContext_.renderersManager_.Subscribe(&thisObject_);
        isSubscribed_ = true;
    }
}
void GrassRendererComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(&thisObject_);
        isSubscribed_ = false;
    }
}
Mesh GrassRendererComponent::CreateGrassMesh(const Material& material) const
{
    Mesh mesh(GraphicsApi::RenderType::POINTS, componentContext_.graphicsApi_, material);
    mesh.GetMeshDataRef().positions_ = positions_;
    return mesh;
}
void GrassRendererComponent::CreateGrassModel()
{
    if (not model_.Get(LevelOfDetail::L1))
    {
        auto model    = std::make_unique<Model>();
        auto material = CreateGrassMaterial();
        auto mesh     = CreateGrassMesh(material);
        model->AddMesh(mesh);
        model_.Add(model.get(), LevelOfDetail::L1);
        componentContext_.resourceManager_.AddModel(std::move(model));
    }
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
