#include "GrassRendererComponent.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <fstream>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryExporter.h"
#include "GameEngine/Resources/Models/Loaders/Binary/BinaryImporter.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace Components
{
template <typename S>
void serialize(S& s, GrassRendererComponent::Ssbo& ssbo)
{
    s.object(ssbo.position.value);
    s.object(ssbo.rotation.value);
    s.object(ssbo.normal.value);
    s.object(ssbo.colorAndSizeRandomness.value);
}
template <typename S>
void serialize(S& s, std::vector<GrassRendererComponent::Ssbo>& ssbos)
{
    constexpr size_t MAX_SIZE = std::numeric_limits<size_t>::max();
    s.container(ssbos, MAX_SIZE);
}
namespace
{
void ExportSSBO(const std::vector<GrassRendererComponent::Ssbo>& ssbos, const std::filesystem::path& path)
{
    auto outputPath = path;
    outputPath.replace_extension(".bin");

    std::ofstream file(outputPath, std::ios::binary);
    if (not file)
    {
        LOG_WARN << "Open file error: " << outputPath;
        return;
    }
    bitsery::Serializer<bitsery::OutputStreamAdapter> ser{file};
    ser.object(ssbos);

    if (not file.good())
    {
        LOG_ERROR << "File write error: " << outputPath;
        return;
    }

    LOG_DEBUG << "Export done: " << outputPath;
}
std::vector<GrassRendererComponent::Ssbo> ImportSSBO(const std::filesystem::path& path)
{
    std::vector<GrassRendererComponent::Ssbo> result;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        LOG_ERROR << "Failed to open binary model file: " << path;
        return {};
    }

    bitsery::Deserializer<bitsery::InputStreamAdapter> des{file};
    des.object(result);

    if (not file.good())
    {
        LOG_ERROR << "Binary model import failed: " << path;
        return {};
    }

    return result;
}
}  // namespace

GrassRendererComponent::GrassRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<GrassRendererComponent>(), componentContext, gameObject)
    , textureFile("")
    , isSubscribed_(false)
{
}
void GrassRendererComponent::CleanUp()
{
    UnSubscribe();
    if (ssbo)
    {
        componentContext_.gpuResourceLoader_.AddObjectToRelease(std::move(ssbo));
    }

    CleanUpMaterial();
}
void GrassRendererComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&GrassRendererComponent::OnAwake, this));
}
void GrassRendererComponent::Reload()
{
    CleanUp();
    OnAwake();
}
GrassRendererComponent& GrassRendererComponent::setDataFile(const File& file)
{
    dataFile = file;
    return *this;
}
GrassRendererComponent& GrassRendererComponent::setTexture(const File& filename)
{
    textureFile = filename;
    if (isSubscribed_)
    {
        CleanUpMaterial();
        material = CreateMaterial();
    }
    return *this;
}
void GrassRendererComponent::UnSubscribe()
{
    if (isSubscribed_)
    {
        componentContext_.renderersManager_.UnSubscribe(*this);
        isSubscribed_ = false;
    }
}
void GrassRendererComponent::CreateDataFile()
{
    dataFile = EngineConf.files.getGeneratedDirPath() / (Utils::CreateUniqueFilename() + ".plantData");
    LOG_DEBUG << "Generatefd data file name: " << dataFile;
}
Material GrassRendererComponent::CreateMaterial() const
{
    if (not textureFile.exist())
    {
        LOG_WARN << "TextureFile not exist :" << textureFile;
        return {};
    }

    GameEngine::TextureParameters tp;
    tp.mimap = GraphicsApi::TextureMipmap::LINEAR;

    Material material;
    material.diffuseTexture = componentContext_.resourceManager_.GetTextureLoader().LoadTexture(textureFile, tp);
    return material;
}
void GrassRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<GrassRendererComponent>(componentContext, gameObject);

        if (auto textureFileNameNode = node.getChild(CSTR_TEXTURE_FILENAME))
        {
            component->setTexture(textureFileNameNode->value_);
        }

        if (auto filenameNode = node.getChild(CSTR_FILE_NAME))
        {
            component->setDataFile(filenameNode->value_);
        }
        return component;
    };
    regsiterComponentReadFunction(GetComponentType<GrassRendererComponent>(), readFunc);
}
void GrassRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    ::write(node.addChild(CSTR_FILE_NAME), dataFile.GetDataRelativePath());
    if (not dataFile.empty() and ssbo)
    {
        ExportSSBO(ssbo->GetData(), dataFile.GetAbsolutePath());
    }

    ::write(node.addChild(CSTR_TEXTURE_FILENAME), getTextureFile().GetDataRelativePath());
}
void GrassRendererComponent::SubscribeToRenderer()
{
    if (isSubscribed_)
    {
        LOG_WARN << "Already subscribed to renderer.";
        return;
    }
    componentContext_.renderersManager_.Subscribe(&thisObject_);
    isSubscribed_ = true;
}
void GrassRendererComponent::CreateSsbo()
{
    if (not ssbo)
    {
        if (dataFile.empty())
        {
            dataFile = EngineConf.files.getGeneratedDirPath() / ("TreeLeaf_" + Utils::CreateUniqueFilename() + ".bin");
        }

        LOG_DEBUG << "Create shaderStorageVectorBufferObject";
        ssbo = std::make_unique<ShaderStorageVectorBufferObject<Ssbo>>(componentContext_.resourceManager_.GetGraphicsApi(),
                                                                       PER_INSTANCES_BIND_LOCATION);
    }
}
void GrassRendererComponent::UpdateSsbo(std::vector<Ssbo>&& v)
{
    if (dataFile.empty())
    {
        dataFile = EngineConf.files.getGeneratedDirPath() / ("TreeLeaf_" + Utils::CreateUniqueFilename() + ".bin");
    }

    if (not ssbo)
    {
        CreateSsbo();
    }
    ssbo->GetData() = std::move(v);
    componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*ssbo);
}
const Material& GrassRendererComponent::GetMaterial() const
{
    return material;
}
size_t GrassRendererComponent::GetCount() const
{
    return ssbo ? ssbo->GetCount() : 0;
}
const GraphicsApi::ID& GrassRendererComponent::GetSsboId() const
{
    if (not ssbo)
    {
        static GraphicsApi::ID id;
        return id;
    }

    return ssbo->GetGraphicsObjectId();
}
void GrassRendererComponent::AddInstance(const Ssbo& v)
{
    if (ssbo)
    {
        ssbo->GetData().push_back(v);
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*ssbo);
    }
    else
    {
        std::vector<Ssbo> newSsboVector{v};
        UpdateSsbo(std::move(newSsboVector));
    }
}
void GrassRendererComponent::AddInstances(std::vector<Ssbo>&& v)
{
    if (ssbo)
    {
        ssbo->GetData().insert(ssbo->GetData().end(), v.begin(), v.end());
        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*ssbo);
    }
    else
    {
        UpdateSsbo(std::move(v));
    }
}
const GrassRendererComponent::SsboType& GrassRendererComponent::GetSsbo() const
{
    return ssbo;
}
void GrassRendererComponent::RemoveInstances(const std::vector<vec3>& positionsToRemove)
{
    if (ssbo)
    {
        auto& dataVector = ssbo->GetData();

        const float epsilon = 0.0001f;

        dataVector.erase(std::remove_if(dataVector.begin(), dataVector.end(),
                                        [&](const Ssbo& instance)
                                        {
                                            vec3 instancePos = vec3(instance.position.value);
                                            for (const auto& targetPos : positionsToRemove)
                                            {
                                                if (glm::distance(instancePos, targetPos) < epsilon)
                                                {
                                                    return true;
                                                }
                                            }
                                            return false;
                                        }),
                         dataVector.end());

        componentContext_.resourceManager_.GetGpuResourceLoader().AddObjectToUpdateGpuPass(*ssbo);
    }
}
void GrassRendererComponent::OnAwake()
{
    CreateSsbo();
    if (textureFile)
    {
        material = CreateMaterial();
    }
    if (dataFile)
    {
        auto data = ImportSSBO(dataFile.GetAbsolutePath());
        UpdateSsbo(std::move(data));
    }
    SubscribeToRenderer();
}
void GrassRendererComponent::CleanUpMaterial()
{
    if (material.diffuseTexture)
    {
        componentContext_.resourceManager_.GetTextureLoader().DeleteTexture(*material.diffuseTexture);
        material.diffuseTexture = nullptr;
    }
}
}  // namespace Components
}  // namespace GameEngine
