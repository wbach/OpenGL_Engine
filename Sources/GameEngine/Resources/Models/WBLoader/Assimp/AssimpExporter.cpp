#include "AssimpExporter.h"

#include <GraphicsApi/MeshRawData.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

#include <assimp/Exporter.hpp>
#include <vector>

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "Logger/Log.h"

namespace GameEngine
{
aiMaterial* CreateAiMaterialFromGameMaterial(const Material& mat)
{
    aiMaterial* aiMat = new aiMaterial();

    // Nazwa
    aiString name(mat.name.c_str());
    aiMat->AddProperty(&name, AI_MATKEY_NAME);

    aiColor3D diffuse(mat.diffuse.value.x, mat.diffuse.value.y, mat.diffuse.value.z);
    aiColor3D ambient(mat.ambient.value.x, mat.ambient.value.y, mat.ambient.value.z);
    aiColor3D specular(mat.specular.value.x, mat.specular.value.y, mat.specular.value.z);

    aiMat->AddProperty(&diffuse, 1, AI_MATKEY_COLOR_DIFFUSE);
    aiMat->AddProperty(&ambient, 1, AI_MATKEY_COLOR_AMBIENT);
    aiMat->AddProperty(&specular, 1, AI_MATKEY_COLOR_SPECULAR);

    // ShineDamper → glossiness
    float shininess = mat.shineDamper;
    aiMat->AddProperty(&shininess, 1, AI_MATKEY_SHININESS);

    // Tekstury
    if (mat.diffuseTexture && mat.diffuseTexture->GetFile().has_value())
    {
        aiString texPath(mat.diffuseTexture->GetFile()->GetAbsolutePath().c_str());
        aiMat->AddProperty(&texPath, AI_MATKEY_TEXTURE_DIFFUSE(0));
    }
    if (mat.specularTexture && mat.specularTexture->GetFile().has_value())
    {
        aiString texPath(mat.specularTexture->GetFile()->GetAbsolutePath().c_str());
        aiMat->AddProperty(&texPath, AI_MATKEY_TEXTURE_SPECULAR(0));
    }
    if (mat.normalTexture && mat.normalTexture->GetFile().has_value())
    {
        aiString texPath(mat.normalTexture->GetFile()->GetAbsolutePath().c_str());
        aiMat->AddProperty(&texPath, AI_MATKEY_TEXTURE_NORMALS(0));
    }
    if (mat.displacementTexture && mat.displacementTexture->GetFile().has_value())
    {
        aiString texPath(mat.displacementTexture->GetFile()->GetAbsolutePath().c_str());
        aiMat->AddProperty(&texPath, AI_MATKEY_TEXTURE_HEIGHT(0));
    }

    return aiMat;
}

void AddBonesToMesh(aiMesh* mesh, const GraphicsApi::MeshRawData& src, size_t maxBonesPerVertex = 4)
{
    size_t vertexCount = mesh->mNumVertices;
    std::vector<int> uniqueBoneIds(src.joinIds_.begin(), src.joinIds_.end());
    std::sort(uniqueBoneIds.begin(), uniqueBoneIds.end());
    uniqueBoneIds.erase(std::unique(uniqueBoneIds.begin(), uniqueBoneIds.end()), uniqueBoneIds.end());

    mesh->mNumBones = static_cast<unsigned int>(uniqueBoneIds.size());
    mesh->mBones    = new aiBone*[mesh->mNumBones];

    for (size_t i = 0; i < uniqueBoneIds.size(); ++i)
    {
        int boneId        = uniqueBoneIds[i];
        aiBone* bone      = new aiBone();
        bone->mName       = aiString("Bone_" + std::to_string(boneId));
        bone->mNumWeights = static_cast<unsigned int>(vertexCount);  // wagi na wszystkich wierzchołkach
        bone->mWeights    = new aiVertexWeight[vertexCount];

        for (size_t v = 0; v < vertexCount; ++v)
        {
            float weight = 0.0f;
            for (size_t b = 0; b < maxBonesPerVertex; ++b)
            {
                if (src.joinIds_[v * maxBonesPerVertex + b] == boneId)
                    weight = src.bonesWeights_[v * maxBonesPerVertex + b];
            }
            bone->mWeights[v].mVertexId = static_cast<unsigned int>(v);
            bone->mWeights[v].mWeight   = weight;
        }

        bone->mOffsetMatrix = aiMatrix4x4();
        mesh->mBones[i]     = bone;
    }
}

aiMesh* CreateAiMeshFromMeshRawData(const GraphicsApi::MeshRawData& src)
{
    aiMesh* mesh          = new aiMesh();
    mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

    const size_t vertexCount = src.positions_.size() / 3;
    mesh->mNumVertices       = static_cast<unsigned int>(vertexCount);
    mesh->mVertices          = new aiVector3D[vertexCount];
    for (size_t i = 0; i < vertexCount; ++i)
        mesh->mVertices[i] = {src.positions_[i * 3 + 0], src.positions_[i * 3 + 1], src.positions_[i * 3 + 2]};

    if (!src.normals_.empty())
    {
        mesh->mNormals = new aiVector3D[vertexCount];
        for (size_t i = 0; i < vertexCount; ++i)
            mesh->mNormals[i] = {src.normals_[i * 3 + 0], src.normals_[i * 3 + 1], src.normals_[i * 3 + 2]};
    }

    if (!src.textCoords_.empty())
    {
        mesh->mTextureCoords[0]   = new aiVector3D[vertexCount];
        mesh->mNumUVComponents[0] = 2;
        for (size_t i = 0; i < vertexCount; ++i)
            mesh->mTextureCoords[0][i] = {src.textCoords_[i * 2 + 0], src.textCoords_[i * 2 + 1], 0.0f};
    }

    if (!src.tangents_.empty() && !src.bitangents_.empty())
    {
        mesh->mTangents   = new aiVector3D[vertexCount];
        mesh->mBitangents = new aiVector3D[vertexCount];
        for (size_t i = 0; i < vertexCount; ++i)
        {
            mesh->mTangents[i]   = {src.tangents_[i * 3 + 0], src.tangents_[i * 3 + 1], src.tangents_[i * 3 + 2]};
            mesh->mBitangents[i] = {src.bitangents_[i * 3 + 0], src.bitangents_[i * 3 + 1], src.bitangents_[i * 3 + 2]};
        }
    }

    if (!src.bonesWeights_.empty() && !src.joinIds_.empty())
        AddBonesToMesh(mesh, src, 4);

    const size_t faceCount = src.indices_.size() / 3;
    mesh->mNumFaces        = static_cast<unsigned int>(faceCount);
    mesh->mFaces           = new aiFace[faceCount];
    for (size_t i = 0; i < faceCount; ++i)
    {
        aiFace& face     = mesh->mFaces[i];
        face.mNumIndices = 3;
        face.mIndices    = new unsigned int[3]{static_cast<unsigned int>(src.indices_[i * 3 + 0]),
                                               static_cast<unsigned int>(src.indices_[i * 3 + 1]),
                                               static_cast<unsigned int>(src.indices_[i * 3 + 2])};
    }
    return mesh;
}

aiScene* CreateSceneFromModel(const Model& model)
{
    aiScene* scene   = new aiScene();
    scene->mRootNode = new aiNode();

    size_t meshCount     = model.GetMeshes().size();
    scene->mMeshes       = new aiMesh*[meshCount];
    scene->mMaterials    = new aiMaterial*[meshCount];
    scene->mNumMeshes    = static_cast<unsigned int>(meshCount);
    scene->mNumMaterials = static_cast<unsigned int>(meshCount);

    scene->mRootNode->mMeshes    = new unsigned int[meshCount];
    scene->mRootNode->mNumMeshes = static_cast<unsigned int>(meshCount);

    for (size_t i = 0; i < meshCount; ++i)
    {
        const Mesh& mesh = model.GetMeshes()[i];
        const auto& raw  = mesh.GetCMeshDataRef();

        aiMesh* aiMeshPtr         = CreateAiMeshFromMeshRawData(raw);
        aiMeshPtr->mMaterialIndex = static_cast<unsigned int>(i);
        scene->mMeshes[i]         = aiMeshPtr;

        aiMaterial* aiMat    = CreateAiMaterialFromGameMaterial(mesh.GetMaterial());
        scene->mMaterials[i] = aiMat;

        scene->mRootNode->mMeshes[i] = static_cast<unsigned int>(i);
    }

    return scene;
}

std::vector<std::string> GetAvaiableFormats()
{
    std::vector<std::string> result;
    Assimp::Exporter exporter;
    for (size_t i = 0; i < exporter.GetExportFormatCount(); ++i)
    {
        auto* f = exporter.GetExportFormatDescription(i);
        result.push_back(f->description);
    }

    return result;
}

// std::string format = binary ? "glb" : "gltf";
bool ExportModel(const Model& model, const File& file)
{
    auto format = file.GetExtension();
    if (not format.empty() and format[0] == '.')
        format = format.substr(1);

    LOG_DEBUG << "Export model to file : " << file << ", format: " << format;

    aiScene* scene = CreateSceneFromModel(model);

    Assimp::Exporter exporter;
    aiReturn result = exporter.Export(scene, format, file.GetAbsolutePath());

    delete scene;

    if (result != aiReturn_SUCCESS)
    {
        LOG_ERROR << "Assimp export failed: " << exporter.GetErrorString();
    }
    return result == aiReturn_SUCCESS;
}
}  // namespace GameEngine
