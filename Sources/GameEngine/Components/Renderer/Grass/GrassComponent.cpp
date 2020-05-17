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
        CopyDataToMesh(mesh);

        auto& meshData = mesh.GetMeshDataRef();
        if (mesh.GetGraphicsObjectId())
        {
            componentContext_.gpuResourceLoader_.AddFunctionToCall(
                [& graphicsApi = this->componentContext_.graphicsApi_, &mesh, &meshData]() {
                    graphicsApi.UpdateMesh(*mesh.GetGraphicsObjectId(), meshData,
                                           {VertexBufferObjects::POSITION, VertexBufferObjects::TEXT_COORD,
                                            VertexBufferObjects::NORMAL, VertexBufferObjects::TANGENT});
                });
        }
    }
    else
    {
        ERROR_LOG("Mesh not set!.");
        return;
    }
}

void GrassRendererComponent::AddGrassMesh(const GrassMeshData& mesh)
{
    meshData_.positions.push_back(mesh.position.x);
    meshData_.positions.push_back(mesh.position.y);
    meshData_.positions.push_back(mesh.position.z);

    meshData_.sizesAndRotations.push_back(mesh.sizeAndRotation.x);
    meshData_.sizesAndRotations.push_back(mesh.sizeAndRotation.y);

    meshData_.normals.push_back(mesh.normal.x);
    meshData_.normals.push_back(mesh.normal.y);
    meshData_.normals.push_back(mesh.normal.z);

    meshData_.colors.push_back(mesh.color.value.x);
    meshData_.colors.push_back(mesh.color.value.y);
    meshData_.colors.push_back(mesh.color.value.z);
}

GrassRendererComponent& GrassRendererComponent::SetMeshesData(GrassMeshes data)
{
    meshData_ = std::move(data);
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
    CopyDataToMesh(mesh);
    return mesh;
}
void GrassRendererComponent::CopyDataToMesh(Mesh& mesh) const
{
    auto& data       = mesh.GetMeshDataRef();
    data.positions_  = meshData_.positions;
    data.textCoords_ = meshData_.sizesAndRotations;
    data.normals_    = meshData_.normals;
    data.tangents_   = meshData_.colors;
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
