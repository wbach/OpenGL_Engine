#include "ResourceManager.h"

#include <Logger/Log.h>

#include <filesystem>
#include <magic_enum/magic_enum.hpp>

#include "IGpuResourceLoader.h"
#include "Models/Primitive.h"
#include "ResourceUtils.h"

namespace GameEngine
{
namespace
{
GraphicsApi::MeshRawData GeneratePrimitive(PrimitiveType type)
{
    switch (type)
    {
        case PrimitiveType::Cube:
            return GenerateCube();
        case PrimitiveType::Sphere:
            return GenerateSphere();
        case PrimitiveType::Cylinder:
            return GenerateCylinder();
        case PrimitiveType::Cone:
            return GenerateCone();
        case PrimitiveType::Plane:
            return GeneratePlane();
        case PrimitiveType::Torus:
            return GenerateTorus();
        case PrimitiveType::Pyramid:
            return GeneratePyramid();
        case PrimitiveType::IcoSphere:
            return GenerateIcoSphere();
        case PrimitiveType::Triangle:
            return GenerateTriangle();
        default:
            return GraphicsApi::MeshRawData();
    }

    return GraphicsApi::MeshRawData();
}
}  // namespace
ResourceManager::ResourceManager(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuResourceLoader,
                                 std::unique_ptr<ITextureLoader> textureLoader,
                                 std::unique_ptr<IModelLoaderFactory> modelLoaderFactory)
    : graphicsApi_(graphicsApi)
    , gpuResourceLoader_(gpuResourceLoader)
    , textureLoader_(std::move(textureLoader))
    , loaderManager_(std::move(modelLoaderFactory))
    , unknowFileNameResourceId_(0)
    , releaseLockState_(false)
{
}

ResourceManager::~ResourceManager()
{
    std::vector<Model*> toRelease;
    for (auto& model : models_)
        toRelease.push_back(model.second.resource_.get());

    LOG_DEBUG << "Release not deleted models. size :" << toRelease.size();

    for (auto model : toRelease)
        ReleaseModel(*model);

    textureLoader_.reset();
}

Model* ResourceManager::LoadModel(const File& file, const LoadingParameters& loadingParameters)
{
    auto absoultePath = file.GetAbsolutePath();
    std::lock_guard<std::mutex> lk(modelMutex_);
    auto iter = models_.find(absoultePath.string());

    if (iter != models_.end())
    {
        auto& modelInfo = iter->second;
        ++modelInfo.instances_;
        modelInfo.resourceGpuStatus_ = ResourceGpuStatus::Loaded;
        // ResourceGpuStatus::NotLoaded for models not implmented. T
        // To do: Can be useful for simplified physics collision mesh shapes.
        // Visual representation of physics shape not needed

        LOG_DEBUG << file.GetBaseName() << " model already loaded, instances count : " << modelInfo.instances_;
        return modelInfo.resource_.get();
    }

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = loaderManager_.Load(file, loadingParameters);

    if (not modelInfo.resource_)
        return nullptr;

    auto modelPtr = modelInfo.resource_.get();
    models_.insert({absoultePath.string(), std::move(modelInfo)});

    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
    return modelPtr;
}

Model* ResourceManager::AddModel(std::unique_ptr<Model> model)
{
    if (not model)
        return nullptr;

    std::lock_guard<std::mutex> lk(modelMutex_);

    auto modelPtr = model.get();
    std::string filename;

    if (not model->GetFile() or model->GetFile().empty())
    {
        filename = ("UnknowFileModel_" + std::to_string(unknowFileNameResourceId_++));
        unknownModelsFilenames_.insert({model.get(), filename});
    }
    else
    {
        filename = model->GetFile().GetAbsolutePath().string();
    }

    auto iter = models_.find(filename);

    if (iter != models_.end())
    {
        auto& modelInfo = iter->second;
        ++modelInfo.instances_;
        modelInfo.resourceGpuStatus_ = ResourceGpuStatus::Loaded;
        LOG_DEBUG << filename << " model already loaded, instances count : " << modelInfo.instances_;
        return modelInfo.resource_.get();
    }

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = std::move(model);

    models_.insert({filename, std::move(modelInfo)});
    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
    return modelPtr;
}

Primitive* ResourceManager::GetPrimitives(PrimitiveType type)
{
    std::lock_guard<std::mutex> lk(modelMutex_);

    auto name = std::string(magic_enum::enum_name(type)) + ".primitive";

    auto iter = models_.find(name);
    if (iter != models_.end())
    {
        auto& modelInfo = iter->second;
        ++modelInfo.instances_;
        modelInfo.resourceGpuStatus_ = ResourceGpuStatus::Loaded;
        return dynamic_cast<Primitive*>(modelInfo.resource_.get());
    }

    auto primitiveData = GeneratePrimitive(type);

    auto model    = std::make_unique<GameEngine::Primitive>(type);
    auto modelPtr = model.get();
    GameEngine::Material material;
    material.diffuse = vec3(0.8f, 0.8f, 0.8f);
    model->AddMesh(GameEngine::Mesh(GraphicsApi::RenderType::TRIANGLES, graphicsApi_, primitiveData, material));

    ResourceInfo<Model> modelInfo;
    modelInfo.resource_ = std::move(model);

    models_.insert({name, std::move(modelInfo)});
    gpuResourceLoader_.AddObjectToGpuLoadingPass(*modelPtr);
    return modelPtr;
}

void ResourceManager::ReleaseModel(Model& model)
{
    std::lock_guard<std::mutex> lk(modelMutex_);

    if (releaseLockState_)
    {
        LOG_DEBUG << "Release resources locked";
        return;
    }

    std::filesystem::path absoultePath;
    if (model.GetFile().empty())
    {
        auto iter = unknownModelsFilenames_.find(&model);
        if (iter != unknownModelsFilenames_.end())
        {
            absoultePath = iter->second;
            unknownModelsFilenames_.erase(iter);
        }
    }
    else
    {
        absoultePath = model.GetFile().GetAbsolutePath();

        if (not models_.count(absoultePath.string()))
        {
            LOG_WARN << "Model dosent exist! " << absoultePath;
            return;
        }
    }

    auto& modelInfo = models_.at(absoultePath.string());
    --modelInfo.instances_;

    if (modelInfo.instances_ > 0)
    {
        LOG_DEBUG << "Model " << absoultePath << " not released. Still using by others. Instances=" << modelInfo.instances_;
        return;
    }

    for (auto& mesh : modelInfo.resource_->GetMeshes())
    {
        DeleteMaterial(mesh.GetMaterial());
    }

    LOG_DEBUG << "Release model :" << absoultePath.string();
    gpuResourceLoader_.AddObjectToRelease(std::move(modelInfo.resource_));
    models_.erase(absoultePath.string());
}

void ResourceManager::LockReleaseResources()
{
    releaseLockState_ = true;
    textureLoader_->LockReleaseResources();
}

void ResourceManager::UnlockReleaseResources()
{
    releaseLockState_ = false;
    textureLoader_->UnlockReleaseResources();
}

void ResourceManager::DeleteMaterial(const Material& material)
{
    if (material.diffuseTexture)
        textureLoader_->DeleteTexture(*material.diffuseTexture);
    if (material.ambientTexture)
        textureLoader_->DeleteTexture(*material.ambientTexture);
    if (material.normalTexture)
        textureLoader_->DeleteTexture(*material.normalTexture);
    if (material.specularTexture)
        textureLoader_->DeleteTexture(*material.specularTexture);
}
}  // namespace GameEngine
