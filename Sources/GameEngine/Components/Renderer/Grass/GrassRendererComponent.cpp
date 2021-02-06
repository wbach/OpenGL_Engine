#include "GrassRendererComponent.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"GrassRenderer"};
}  // namespace

GrassRendererComponent::GrassRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(GrassRendererComponent).hash_code(), componentContext, gameObject)
    , textureFile_("Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png")
    , meshDataFile_(EngineConf.files.data + "/grassMeshData.bin")
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

        if (mesh.GetGraphicsObjectId())
        {
            CopyDataToMesh(mesh);
            auto& meshData = mesh.GetMeshDataRef();

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
        CreateGrassModel();
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

GrassRendererComponent& GrassRendererComponent::setMeshDataFile(const File& file)
{
    meshDataFile_ = file;
    return *this;
}

GrassRendererComponent& GrassRendererComponent::setTexture(const File& filename)
{
    textureFile_ = filename;
    DEBUG_LOG(filename.GetAbsoultePath());
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
    if (not model_.Get(LevelOfDetail::L1))
    {
        CreateGrassModel();
    }

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
    Mesh mesh(GraphicsApi::RenderType::POINTS, componentContext_.graphicsApi_, GraphicsApi::MeshRawData(), material);
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
    if (meshData_.positions.empty())
        return;

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
void GrassRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<GrassRendererComponent>(componentContext, gameObject);
        auto textureFileNameNode = node.getChild(CSTR_TEXTURE_FILENAME);
        if (textureFileNameNode)
        {
            component->setTexture(textureFileNameNode->value_);
        }

        if (node.getChild(CSTR_FILE_NAME))
        {
            Components::GrassRendererComponent::GrassMeshes meshesData;

            auto filename = node.getChild(CSTR_FILE_NAME)->value_;

            if (not filename.empty())
            {
                File file(filename);
                auto succes = file.openToRead();
                if (succes)
                {
                    file.readVectorFromFile(meshesData.positions);
                    file.readVectorFromFile(meshesData.normals);
                    file.readVectorFromFile(meshesData.colors);
                    file.readVectorFromFile(meshesData.sizesAndRotations);
                    file.close();
                }
            }
            component->SetMeshesData(std::move(meshesData));
        }
        return component;
    };
    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}
void GrassRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});

    const auto& meshData = GetGrassMeshesData();

    auto file = getDataFile();
    if (file.empty())
        file.DataRelative("Generated/grassMeshData_" + std::to_string(getParentGameObject().GetId()) + ".bin");

    auto opened = file.openToWrite();
    if (opened)
    {
        file.addVectorToFile(meshData.positions);
        file.addVectorToFile(meshData.normals);
        file.addVectorToFile(meshData.colors);
        file.addVectorToFile(meshData.sizesAndRotations);
        file.close();
    }

    ::write(node.addChild(CSTR_FILE_NAME), file.GetDataRelativeDir());
    ::write(node.addChild(CSTR_TEXTURE_FILENAME), getTextureFile().GetDataRelativeDir());
}
}  // namespace Components
}  // namespace GameEngine
