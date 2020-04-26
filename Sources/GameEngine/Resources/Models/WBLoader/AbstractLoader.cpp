#include "AbstractLoader.h"
#include <fstream>
#include "Binary/BinaryReader.h"
#include "Binary/BinaryWriter.h"
#include "GameEngine/Animations/AnimationUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Logger/Log.h"
#include "Utils/Utils.h"

namespace GameEngine
{
namespace WBLoader
{
AbstractLoader::AbstractLoader(GraphicsApi::IGraphicsApi& graphicsApi, ITextureLoader& textureLoader)
    : textureLoader_(textureLoader)
    , graphicsApi_(graphicsApi)
    , loadedFromBin_(false)
{
}
void AbstractLoader::Parse(const std::string& filename)
{
    fileName_ = Utils::GetFilenameWithExtension(filename);
    filePath_ = Utils::GetFilePath(filename);

    auto binFile = CreateBinPath(fileName_);
    if (EngineConf.useBinaryLoading && Utils::CheckFileExist(binFile))
    {
        ReadBinFile(binFile, textureLoader_);
        loadedFromBin_ = true;
        fileName_      = binFile;
        DEBUG_LOG("Load from bin file :" + filename);
    }
    else
    {
        ParseFile(filename);
    }
}
std::unique_ptr<Model> AbstractLoader::Create()
{
    auto startTime = std::chrono::high_resolution_clock::now();

    std::unique_ptr<Model> newModel;

    if (loadedFromBin_)
    {
        newModel       = CreateModelFromBin();
        loadedFromBin_ = false;
    }
    else
    {
        newModel = CreateModel();
        CreateBinFile(newModel, fileName_);
    }

    auto endTime  = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    DEBUG_LOG("Model created. " + fileName_ + ". Time : " + std::to_string(duration) + "ms. Meshes : " + std::to_string(newModel->GetMeshes().size()) );
    return newModel;
}
std::unique_ptr<Model> AbstractLoader::CreateModel()
{
    float maxFactor = FindMaxFactor();
    DEBUG_LOG(std::to_string(1.f / maxFactor));

    auto newModel = std::make_unique<Model>(maxFactor);

    if (objects.empty())
    {
        ERROR_LOG(fileName_ + ". No object to create!");
    }
    for (auto& obj : objects)
    {
        if (obj.meshes.empty())
        {
            ERROR_LOG(fileName_ + ". No meshes in object!");
        }

        NormalizeMatrix(obj.transformMatrix, maxFactor);

        for (auto& mesh : obj.meshes)
        {
            auto& newMesh = newModel->AddMesh(GraphicsApi::RenderType::TRIANGLES, graphicsApi_, mesh.material,
                obj.transformMatrix);

            IndexinVBO(mesh.vertexBuffer, newMesh.GetMeshDataRef());
            newMesh.SetUseArmatorIfHaveBones();
            newModel->animationClips_ = mesh.animationClips_;
            newModel->skeleton_       = mesh.skeleton_;
            Animation::CalcInverseBindTransform(newModel->skeleton_, mat4(1.f));
        }
    }
    objects.clear();
    return newModel;
}

std::unique_ptr<Model> AbstractLoader::CreateModelFromBin()
{
    return ReadBinFile(fileName_, textureLoader_);
}
void AbstractLoader::NormalizeMatrix(mat4& mat, float factor) const
{
    mat *= glm::scale(vec3(1.f / factor));
}
float AbstractLoader::FindMaxFactor() const
{
    float maxFactor = -std::numeric_limits<float>::max();

    for (auto& obj : objects)
    {
        for (auto& mesh : obj.meshes)
        {
            float f = mesh.GetScaleFactor();
            if (f > maxFactor)
                maxFactor = f;
        }
    }
    return maxFactor;
}
}  // namespace WBLoader
}  // namespace GameEngine
