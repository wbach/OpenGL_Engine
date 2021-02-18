#include "GrassRendererComponent.h"

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"GrassRenderer"};
const int32 BOX_SIZE = 100;
const bool DEVIDE_SPACE{false};
}  // namespace

GrassRendererComponent::GrassRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(GrassRendererComponent).hash_code(), componentContext, gameObject)
    , textureFile_("Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png")
    , meshDataFile_()
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
        auto& mesh = model->GetMeshes().front();

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
    if (EngineConf.renderer.flora.isEnabled)
    {
        if (not isSubscribed_ and not model_.Get(LevelOfDetail::L1))
        {
            if (CreateGrassModel())
            {
                componentContext_.renderersManager_.Subscribe(&thisObject_);
                isSubscribed_ = true;
            }
            else
            {
                ERROR_LOG("Model creation error.");
            }
        }
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
std::vector<Mesh> GrassRendererComponent::CreateGrassMeshes(const Material& material) const
{
    std::vector<Mesh> result;
    if (DEVIDE_SPACE)
    {
        std::unordered_map<int32, std::unordered_map<int32, std::unordered_map<int32, Mesh>>> meshBoxes_;
        size_t sizeAndRotationIndex = 0;
        for(size_t i = 0; i< meshData_.positions.size(); i += 3)
        {
            vec3 position(meshData_.positions[i], meshData_.positions[i +1], meshData_.positions[i+2]);

            auto xIndex = static_cast<int32>(position.x) / BOX_SIZE;
            auto yIndex = static_cast<int32>(position.y) / BOX_SIZE;
            auto zIndex = static_cast<int32>(position.z) / BOX_SIZE;

            auto& yMeshBox = meshBoxes_[xIndex][yIndex];
            auto iter = yMeshBox.find(zIndex);

            Mesh* mesh{nullptr};
            if (iter == yMeshBox.end())
            {
                Mesh m(GraphicsApi::RenderType::POINTS, componentContext_.graphicsApi_, GraphicsApi::MeshRawData(), material);
                yMeshBox.insert({zIndex, std::move(m) });
                mesh = &yMeshBox.at(zIndex);
                vec3 bbMin(xIndex * BOX_SIZE, yIndex * BOX_SIZE, zIndex * BOX_SIZE);
                vec3 bbMax((xIndex+1) * BOX_SIZE, (yIndex+1) * BOX_SIZE, (zIndex+1) * BOX_SIZE);
                DEBUG_LOG("bbMin : " + std::to_string(bbMin));
                DEBUG_LOG("bbMax : " + std::to_string(bbMax));
                BoundingBox boundingBox(bbMin * 1.1f, bbMax * 1.1f);
                mesh->setBoundingBox(boundingBox);
            }
            else
            {
                mesh = &iter->second;
            }

            auto& meshData = mesh->GetMeshDataRef();
            meshData.positions_.push_back(position.x);
            meshData.positions_.push_back(position.y);
            meshData.positions_.push_back(position.z);

            meshData.textCoords_.push_back(meshData_.sizesAndRotations[sizeAndRotationIndex++]);
            meshData.textCoords_.push_back(meshData_.sizesAndRotations[sizeAndRotationIndex++]);

            meshData.normals_.push_back(meshData_.normals[i]);
            meshData.normals_.push_back(meshData_.normals[i+1]);
            meshData.normals_.push_back(meshData_.normals[i+2]);

            meshData.tangents_.push_back(meshData_.colors[i]);
            meshData.tangents_.push_back(meshData_.colors[i+1]);
            meshData.tangents_.push_back(meshData_.colors[i+2]);
        }

        for(auto& [_, yIndexMap] : meshBoxes_)
        {
            for(auto& [_, zIndexMap] : yIndexMap)
            {
                for(auto& [_, mesh] : zIndexMap)
                {
                    DEBUG_LOG("Mesh positions size : " + std::to_string(mesh.GetMeshDataRef().positions_.size() / 3));
                    result.push_back(std::move(mesh));
                }
            }
        }
        DEBUG_LOG("Meshes size : " + std::to_string(result.size()));
    }
    else
    {
        Mesh mesh(GraphicsApi::RenderType::POINTS, componentContext_.graphicsApi_, GraphicsApi::MeshRawData(), material);
        CopyDataToMesh(mesh);
        result.push_back(std::move(mesh));
    }

    return result;
}
void GrassRendererComponent::CopyDataToMesh(Mesh& mesh) const
{
    auto& data       = mesh.GetMeshDataRef();
    data.positions_  = meshData_.positions;
    data.textCoords_ = meshData_.sizesAndRotations;
    data.normals_    = meshData_.normals;
    data.tangents_   = meshData_.colors;
}
bool GrassRendererComponent::CreateGrassModel()
{
    if (meshData_.positions.empty())
        return false;

    auto model    = std::make_unique<Model>();
    auto material = CreateGrassMaterial();
    auto meshes   = CreateGrassMeshes(material);
    for(auto& mesh : meshes)
        model->AddMesh(mesh);

    model_.Add(model.get(), LevelOfDetail::L1);
    model->SetFile(meshDataFile_);
    auto addedModel = componentContext_.resourceManager_.AddModel(std::move(model));
    return addedModel != nullptr;
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

        if (auto textureFileNameNode = node.getChild(CSTR_TEXTURE_FILENAME))
        {
            component->setTexture(textureFileNameNode->value_);
        }

        if (auto filenameNode = node.getChild(CSTR_FILE_NAME))
        {
            Components::GrassRendererComponent::GrassMeshes meshesData;
            auto filename = filenameNode->value_;
            component->setMeshDataFile(filename);

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
        file.DataRelative("Generated/grassMeshData_" + std::to_string(thisObject_.GetId()) + ".bin");

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
