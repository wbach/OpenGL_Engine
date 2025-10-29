#include "TerrainRendererComponent.h"

#include <Logger/Log.h>
#include <Utils/TreeNode.h>
#include <Utils/Variant.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/ConfigurationParams/RendererParams/TerrainParam/TerrainType.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "TerrainConfiguration.h"
#include "TerrainMeshRendererComponent.h"
#include "TerrainTessellationRendererComponent.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string CSTR_TEXTURE_FILENAMES = "textureFileNames";
const std::string CSTR_TEXTURE_TYPE      = "textureType";
}  // namespace

TerrainRendererComponent::RendererType Convert(Params::TerrainType type)
{
    switch (type)
    {
        case Params::TerrainType::Mesh:
            return TerrainRendererComponent::RendererType::Mesh;
        case Params::TerrainType::Tessellation:
            return TerrainRendererComponent::RendererType::Tessellation;
    }

    return TerrainRendererComponent::RendererType::Mesh;
}

TerrainRendererComponent::TerrainRendererComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<TerrainRendererComponent>(), componentContext, gameObject)
    , functionsRegistered_(false)
{
    if (EngineConf.renderer.type == GraphicsApi::RendererType::SIMPLE)
    {
        SetRendererType(RendererType::Mesh);
    }
    else if (not componentContext_.resourceManager_.GetGraphicsApi().IsTesselationSupported())
    {
        SetRendererType(RendererType::Mesh);
        return;
    }
    else
    {
        SetRendererType(Convert(EngineConf.renderer.terrain.terrainType));
    }
}

TerrainRendererComponent::~TerrainRendererComponent()
{
}

void TerrainRendererComponent::CleanUp()
{
    terrainComponent_->CleanUp();
}

TerrainRendererComponent& TerrainRendererComponent::LoadTexture(const TerrainTexture& texture)
{
    terrainComponent_->LoadTexture(texture);
    return *this;
}

TerrainRendererComponent& TerrainRendererComponent::LoadTextures(const std::vector<TerrainTexture>& textures)
{
    terrainComponent_->LoadTextures(textures);
    return *this;
}

std::optional<File> TerrainRendererComponent::ConvertObjectToHeightMap(const File& objectFile, uint32 heightmapResultuion,
                                                                       const std::optional<File>& outputfile) const
{
    return terrainComponent_->ConvertObjectToHeightMap(objectFile, heightmapResultuion, outputfile);
}

Texture* TerrainRendererComponent::GetTexture(TerrainTextureType type) const
{
    return terrainComponent_->GetTexture(type);
}

const std::vector<std::pair<TerrainTextureType, Texture*>>& TerrainRendererComponent::GetTextures() const
{
    return terrainComponent_->GetTextures();
}

void TerrainRendererComponent::ChangeTexture(const File& old, const File& newFile)
{
    terrainComponent_->ChangeTexture(old, newFile);
}

void TerrainRendererComponent::RemoveTexture(const File& file)
{
    terrainComponent_->RemoveTexture(file);
}

void TerrainRendererComponent::setTexture(Texture& texture, float tiledSize, TerrainTextureType type)
{
    terrainComponent_->setTexture(texture, tiledSize, type);
}

const std::vector<TerrainTexture>& TerrainRendererComponent::GetInputDataTextures() const
{
    return terrainComponent_->GetInputDataTextures();
}

void TerrainRendererComponent::UpdateTexture(TerrainTextureType textureType, const std::string& texture)
{
    terrainComponent_->UpdateTexture(textureType, texture);
}

const TerrainConfiguration& TerrainRendererComponent::GetTerrainConfiguration() const
{
    return terrainComponent_->GetConfiguration();
}

void TerrainRendererComponent::SetRendererType(TerrainRendererComponent::RendererType type)
{
    if (terrainComponent_)
    {
        terrainComponent_->CleanUp();
    }

    switch (type)
    {
        case RendererType::Mesh:
            terrainComponent_ = std::make_unique<TerrainMeshRendererComponent>(componentContext_, thisObject_);
            break;
        case RendererType::Tessellation:
            terrainComponent_ = std::make_unique<TerrainTessellationRendererComponent>(componentContext_, thisObject_);
            break;
    }

    if (functionsRegistered_)
    {
        ReqisterFunctions();
    }
    rendererType_ = type;
}

TerrainRendererComponent::RendererType TerrainRendererComponent::GetRendererType() const
{
    return rendererType_;
}

TerrainTessellationRendererComponent* TerrainRendererComponent::GetTesselationTerrain()
{
    if (rendererType_ != RendererType::Tessellation)
    {
        LOG_ERROR << "Get tesselation comonent when rendererType_ != RendererType::Tessellation";
        return nullptr;
    }

    return static_cast<TerrainTessellationRendererComponent*>(terrainComponent_.get());
}

TerrainMeshRendererComponent* TerrainRendererComponent::GetMeshTerrain()
{
    if (rendererType_ != RendererType::Mesh)
    {
        LOG_ERROR << "Get tesselation comonent when rendererType_ != RendererType::Mesh";
        return nullptr;
    }

    return static_cast<TerrainMeshRendererComponent*>(terrainComponent_.get());
}

HeightMap* TerrainRendererComponent::createHeightMap(const vec2ui& size)
{
    return terrainComponent_->createHeightMap(size);
}

HeightMap* TerrainRendererComponent::GetHeightMap()
{
    return terrainComponent_->GetHeightMap();
}

void TerrainRendererComponent::HeightMapChanged()
{
    terrainComponent_->HeightMapChanged();
}

void TerrainRendererComponent::BlendMapChanged()
{
    terrainComponent_->BlendMapChanged();
}

void TerrainRendererComponent::RecalculateNormals()
{
    terrainComponent_->RecalculateNormals();
}

void TerrainRendererComponent::ReqisterFunctions()
{
    const auto functions = terrainComponent_->FunctionsToRegister();
    for (auto f : functions)
    {
        RegisterFunction(f.first, f.second);
    }
    functionsRegistered_ = true;
}

void TerrainRendererComponent::Reload()
{
}

std::vector<Components::TerrainTexture> ReadTerrainTextures(const TreeNode& node)
{
    std::vector<Components::TerrainTexture> result;

    for (const auto& texture : node.getChildren())
    {
        Components::TerrainTexture terrainTexture;
        if (texture->getChild(CSTR_TEXTURE_FILENAME))
            terrainTexture.file = File(texture->getChild(CSTR_TEXTURE_FILENAME)->value_);

        if (texture->getChild(CSTR_TEXTURE_TYPE))
            std::from_string(texture->getChild(CSTR_TEXTURE_TYPE)->value_, terrainTexture.type);
        else
            LOG_WARN << "Read texture without type";

        Read(texture->getChild(CSTR_SCALE), terrainTexture.tiledScale);

        result.push_back(terrainTexture);
    }
    std::sort(result.begin(), result.end(),
              [](const auto& l, const auto& r) { return static_cast<int>(l.type) < static_cast<int>(r.type); });
    return result;
}
void TerrainRendererComponent::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<TerrainRendererComponent>(componentContext, gameObject);

        auto texturesNode = node.getChild(CSTR_TEXTURE_FILENAMES);
        if (texturesNode)
        {
            auto textures = ReadTerrainTextures(*node.getChild(CSTR_TEXTURE_FILENAMES));
            component->LoadTextures(textures);
        }

        if (not component->GetHeightMap())
            component->createHeightMap({1024, 1024});

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<TerrainRendererComponent>(), readFunc);
}

namespace
{
void create(TreeNode& node, TerrainTextureType type, float scale, const std::filesystem::path& filepath)
{
    ::write(node.addChild(CSTR_TEXTURE_TYPE), std::to_string(type));
    ::write(node.addChild(CSTR_SCALE), std::to_string(scale));
    ::write(node.addChild(CSTR_TEXTURE_FILENAME), Utils::ReplaceSlash(filepath.string()));
}

void create(TreeNode& node, const std::vector<Components::TerrainTexture>& textures)
{
    for (const auto& value : textures)
    {
        create(node.addChild(CSTR_TEXTURE), value.type, value.tiledScale, value.file.GetDataRelativePath());
    }
}
}  // namespace

void TerrainRendererComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});

    create(node.addChild(CSTR_TEXTURE_FILENAMES), GetInputDataTextures());

    auto heightMapTexture = GetTexture(TerrainTextureType::heightmap);

    if (heightMapTexture and heightMapTexture->IsModified())
    {
        auto heightMap = static_cast<HeightMap*>(heightMapTexture);
        if (heightMap and heightMap->GetFile())
        {
            SaveHeightMap(*heightMap, heightMap->GetFile()->GetAbsolutePath());
        }
        else
        {
            LOG_ERROR << "Heightmap texture cast error";
        }
    }

    auto blendMapTexture = GetTexture(TerrainTextureType::blendMap);

    if (blendMapTexture)
    {
        if (blendMapTexture->IsModified() and blendMapTexture->GetFile())
        {
            auto blendMap     = static_cast<GeneralTexture*>(blendMapTexture);
            const auto& image = blendMap->GetImage();
            Utils::CreateBackupFile(blendMapTexture->GetFile()->GetAbsolutePath().string());

            std::visit(
                visitor{
                    [&](const std::vector<uint8>& data)
                    { Utils::SaveImage(data, image.size(), blendMapTexture->GetFile()->GetAbsolutePath().string()); },
                    [](const std::vector<float>& data) { LOG_ERROR << "Float version not implemented."; },
                    [](const std::monostate&) { LOG_ERROR << "Image data is not set!"; },
                },
                image.getImageData());
        }
    }
}

}  // namespace Components
}  // namespace GameEngine
