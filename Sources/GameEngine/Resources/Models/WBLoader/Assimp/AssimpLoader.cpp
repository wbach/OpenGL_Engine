#include "AssimpLoader.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <assimp/Importer.hpp>

#include "GLM/GLMUtils.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace WBLoader
{
namespace
{
Quaternion covnertQuat(const aiQuaternion& quat)
{
    Quaternion result;
    result.x = quat.x;
    result.y = quat.y;
    result.z = quat.z;
    result.w = quat.w;
    return result;
}
vec3 covnertVec3(const aiVector3D& v)
{
    vec3 result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    return result;
}
mat4 convert(const aiMatrix4x4& aiMatrix)
{
    glm::mat4 m;
    m[0][0] = aiMatrix.a1;
    m[0][1] = aiMatrix.b1;
    m[0][2] = aiMatrix.c1;
    m[0][3] = aiMatrix.d1;
    m[1][0] = aiMatrix.a2;
    m[1][1] = aiMatrix.b2;
    m[1][2] = aiMatrix.c2;
    m[1][3] = aiMatrix.d2;
    m[2][0] = aiMatrix.a3;
    m[2][1] = aiMatrix.b3;
    m[2][2] = aiMatrix.c3;
    m[2][3] = aiMatrix.d3;
    m[3][0] = aiMatrix.a4;
    m[3][1] = aiMatrix.b4;
    m[3][2] = aiMatrix.c4;
    m[3][3] = aiMatrix.d4;
    return m;
}
}  // namespace
AssimpLoader::AssimpLoader(ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
    auto version = std::to_string(aiGetVersionMajor()) + "." + std::to_string(aiGetVersionMinor());
    DEBUG_LOG("Assimp version : " + version);
}

AssimpLoader::~AssimpLoader()
{
}

void AssimpLoader::ParseFile(const File& file)
{
    uint32 flags = aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_CalcTangentSpace;

    flags |= aiProcess_JoinIdenticalVertices | aiProcess_OptimizeGraph |
             aiProcess_OptimizeMeshes;  // aiProcess_GenSmoothNormals

    Assimp::Importer importer;
    auto scene = importer.ReadFile(file.GetAbsoultePath().c_str(), flags);

    if (not scene)
    {
        ERROR_LOG("import file error : " + importer.GetErrorString());
        return;
    }

    if (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
    {
        WARNING_LOG("import scene incomplete");
    }
    auto globalInverseTransform = scene->mRootNode->mTransformation;
    globalInverseTransform.Inverse();

    objects.push_back(WBLoader::Object());
    recursiveProcess(*scene, *scene->mRootNode);
    processAnimations(*scene);
    importer.FreeScene();
}

bool AssimpLoader::CheckExtension(const File& file)
{
    return file.IsExtension(
        {"AMF",     "3DS",  "AC",      "ASE", "ASSBIN",   "B3D", "BVH",   "COLLADA", "DXF",  "CSM", "DAE", "HMP",
         "IRRMESH", "IRR",  "LWO",     "LWS", "MD2",      "MD3", "MD5",   "MDC",     "MDL",  "NFF", "NDO", "OFF",
         "OBJ",     "OGRE", "OPENGEX", "PLY", "MS3D",     "COB", "BLEND", "IFC",     "XGL",  "FBX", "Q3D", "Q3BSP",
         "RAW",     "SIB",  "SMD",     "STL", "TERRAGEN", "3D",  "X",     "X3D",     "GLTF", "3MF", "MMD", "STEP"});

    // AMF 3DS AC ASE ASSBIN B3D BVH COLLADA DXF CSM HMP IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL NFF NDO OFF OBJ OGRE
    // OPENGEX PLY MS3D COB BLEND IFC XGL FBX Q3D Q3BSP RAW SIB SMD STL TERRAGEN 3D X X3D GLTF 3MF MMD STEP
}

void AssimpLoader::recursiveProcess(const aiScene& scene, const aiNode& node)
{
    for (uint32 i = 0; i < node.mNumMeshes; ++i)
    {
        processMesh(scene, *scene.mMeshes[node.mMeshes[i]]);
    }

    for (uint32 i = 0; i < node.mNumChildren; ++i)
    {
        recursiveProcess(scene, *node.mChildren[i]);
    }
}

void AssimpLoader::processMesh(const aiScene& scene, const aiMesh& mesh)
{
    objects.back().meshes.emplace_back();
    auto& newMesh        = objects.back().meshes.back();
    newMesh.meshRawData_ = GraphicsApi::MeshRawData();

    loadVertexData(mesh, *newMesh.meshRawData_);
    loadBonesData(scene, mesh, newMesh);
    newMesh.material = processMaterial(scene, mesh);
}
void AssimpLoader::loadVertexData(const aiMesh& mesh, GraphicsApi::MeshRawData& meshData)
{
    for (uint32 i = 0; i < mesh.mNumVertices; ++i)
    {
        VertexBuffer vb;

        meshData.positions_.push_back(mesh.mVertices[i].x);
        meshData.positions_.push_back(mesh.mVertices[i].y);
        meshData.positions_.push_back(mesh.mVertices[i].z);

        if (mesh.mTextureCoords[0])
        {
            meshData.textCoords_.push_back(mesh.mTextureCoords[0][i].x);
            meshData.textCoords_.push_back(mesh.mTextureCoords[0][i].y);
        }
        else
        {
            meshData.textCoords_.push_back(0);
            meshData.textCoords_.push_back(0);
        }
        if (mesh.mNormals)
        {
            meshData.normals_.push_back(mesh.mNormals[i].x);
            meshData.normals_.push_back(mesh.mNormals[i].y);
            meshData.normals_.push_back(mesh.mNormals[i].z);
        }
        else
        {
            meshData.normals_.push_back(0);
            meshData.normals_.push_back(0);
            meshData.normals_.push_back(0);
        }
        if (mesh.mTangents)
        {
            meshData.tangents_.push_back(mesh.mTangents[i].x);
            meshData.tangents_.push_back(mesh.mTangents[i].y);
            meshData.tangents_.push_back(mesh.mTangents[i].z);
        }
        else
        {
            meshData.tangents_.push_back(0);
            meshData.tangents_.push_back(0);
            meshData.tangents_.push_back(0);
        }
        if (mesh.mBitangents)
        {
            meshData.bitangents_.push_back(mesh.mBitangents[i].x);
            meshData.bitangents_.push_back(mesh.mBitangents[i].y);
            meshData.bitangents_.push_back(mesh.mBitangents[i].z);
        }
        else
        {
            meshData.bitangents_.push_back(0);
            meshData.bitangents_.push_back(0);
            meshData.bitangents_.push_back(0);
        }
    }
    for (uint32 i = 0; i < mesh.mNumFaces; i++)
    {
        aiFace face = mesh.mFaces[i];
        for (uint32 j = 0; j < face.mNumIndices; j++)
        {
            meshData.indices_.push_back(face.mIndices[j]);
        }
    }
}
void AssimpLoader::loadBonesData(const aiScene& scene, const aiMesh& mesh, Mesh& newMesh)
{
    if (mesh.mNumBones == 0)
    {
        return;
    }

    auto& meshData = *newMesh.meshRawData_;

    std::vector<std::vector<JointInfo>> jointInfos;
    jointInfos.resize(mesh.mNumVertices);
    auto vertexCount = mesh.mNumVertices * 4;
    meshData.joinIds_.resize(vertexCount);
    meshData.bonesWeights_.resize(vertexCount);

    BoneIdPool boneIdPool;
    BonesMap bones;
    for (uint32 i = 0; i < mesh.mNumBones; i++)
    {
        std::string name(mesh.mBones[i]->mName.data);
        if (not bones.count(name))
        {
            auto boneId = boneIdPool.nextId();

            BoneInfo info;
            info.id = boneId;
            info.matrix = convert(mesh.mBones[i]->mOffsetMatrix);
            bones.insert({ name, info });
            for (uint32 j = 0; j < mesh.mBones[i]->mNumWeights; j++)
            {
                const auto& mWeights = mesh.mBones[i]->mWeights[j];
                auto vertexId        = mWeights.mVertexId;
                auto weight          = mWeights.mWeight;
                jointInfos[vertexId].push_back({ boneId, weight});
            }

        }
        else
        {
            WARNING_LOG("bone duplicated");
        }
    }

    size_t i = 0;
    for (auto& vb : jointInfos)
    {
        if (vb.size() <= 4)
        {
            for (size_t j = 0; j < vb.size(); ++j)
            {
                auto index = 4 * i + j;

                meshData.joinIds_[index]      = vb[j].id;
                meshData.bonesWeights_[index] = vb[j].weight;
            }
        }
        else
        {
            WARNING_LOG("To many bones per vertex");
        }
        ++i;
    }

    processSkeleton(scene, newMesh, bones, boneIdPool);
}
void AssimpLoader::processSkeleton(const aiScene& scene, Mesh& mesh, const BonesMap& bones, BoneIdPool& boneIdPool)
{
    if (not bones.empty())
    {
        auto armatureNode = findArmatureRootNode(*scene.mRootNode, bones);
        if (armatureNode)
        {
            createSkeleton(*armatureNode, mesh.skeleton_, bones, boneIdPool);
        }
    }
}
void AssimpLoader::createSkeleton(const aiNode& node, Animation::Joint& rootJoint, const BonesMap& bones, BoneIdPool& boneIdPool)
{
    std::string name(node.mName.data);
    Animation::Joint child;

    if (bones.count(name))
    {
        auto& info = bones.at(name);
        rootJoint.id = info.id;

    }
    else
    {
        rootJoint.id = boneIdPool.nextId();
    }

    rootJoint.name = name;
    rootJoint.transform = convert(node.mTransformation);  // *info.matrix;

    for (uint32 i = 0; i < node.mNumChildren; ++i)
    {
        Animation::Joint child;
        createSkeleton(*node.mChildren[i], child, bones, boneIdPool);
        rootJoint.addChild(std::move(child));
    }
}
void AssimpLoader::processAnimations(const aiScene& scene)
{
    for (uint32 i = 0; i < scene.mNumAnimations; ++i)
    {
        const auto& aiAnim = *scene.mAnimations[i];
        std::string name(aiAnim.mName.data);
        if (name.empty())
            name = "noname";
        DEBUG_LOG("Animation : " + name);
        objects.back().animationClips_.insert({name, processAnimation(aiAnim)});
    }
}

Material AssimpLoader::processMaterial(const aiScene& scene, const aiMesh& mesh) const
{
    aiString name;
    aiColor4D diff;
    aiColor4D amb;
    aiColor4D spec;
    float shine_damper;
    float transparent;
    float reflectivity;

    aiMaterial* mat = scene.mMaterials[mesh.mMaterialIndex];
    aiGetMaterialString(mat, AI_MATKEY_NAME, &name);
    aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diff);
    aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &amb);
    aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &spec);
    aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shine_damper);
    aiGetMaterialFloat(mat, AI_MATKEY_REFLECTIVITY, &reflectivity);
    aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &transparent);

    Material material;
    material.name           = std::string(name.C_Str());
    material.ambient        = vec3(amb.r, amb.g, amb.b);
    material.diffuse        = vec3(diff.r, diff.g, diff.b);
    material.specular       = vec3(spec.r, spec.g, spec.b);
    material.shineDamper    = shine_damper;
    material.reflectivity   = reflectivity;
    material.isTransparency = transparent > 0.5f;

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 1)
    {
        DEBUG_LOG("MultiTexturing not supported.");
    }

    for (uint32 i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); ++i)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, i, &path);
        TextureParameters parameters;
        material.diffuseTexture = textureLoader_.LoadTexture(path.C_Str(), parameters);
    }

    for (uint32 i = 0; i < mat->GetTextureCount(aiTextureType_HEIGHT); i++)
    {
        aiString path;
        mat->GetTexture(aiTextureType_HEIGHT, i, &path);
        TextureParameters parameters;
        material.normalTexture = textureLoader_.LoadTexture(path.C_Str(), parameters);
    }

    return material;
}

Animation::AnimationClip AssimpLoader::processAnimation(const aiAnimation& aiAnim)
{
    Animation::AnimationClip clip;
    clip.name = aiAnim.mName.data;

    for (uint32 i = 0; i < aiAnim.mNumChannels; i++)
    {
        const auto& animChannel = *aiAnim.mChannels[i];
        std::string jointName   = animChannel.mNodeName.data;

        for (uint32 i = 0; i < animChannel.mNumRotationKeys; i++)
        {
            const auto& rotationKey              = animChannel.mRotationKeys[i];
            auto& frame                          = getFrameByTimeStamp(clip, static_cast<float>(rotationKey.mTime));
            frame.transforms[jointName].rotation = covnertQuat(rotationKey.mValue);
        }

        for (uint32 i = 0; i < animChannel.mNumRotationKeys; i++)
        {
            const auto& positionKey              = animChannel.mPositionKeys[i];
            auto& frame                          = getFrameByTimeStamp(clip, static_cast<float>(positionKey.mTime));
            frame.transforms[jointName].position = covnertVec3(positionKey.mValue);
        }
    }

    clip.SetLength(static_cast<float>(aiAnim.mDuration));
    return clip;
}

Animation::KeyFrame& AssimpLoader::getFrameByTimeStamp(Animation::AnimationClip& clip, float time)
{
    auto frame = clip.getFrame(time);

    if (not frame)
    {
        Animation::KeyFrame frame;
        frame.timeStamp = time;
        clip.AddFrame(frame);
    }

    return *clip.getFrame(time);
}

const aiNode* AssimpLoader::findArmatureRootNode(const aiNode& node, const BonesMap& bones)
{
    std::string name(node.mName.data);

    if (bones.count(name))
        return &node;

    for (uint32 i = 0; i < node.mNumChildren; ++i)
    {
        auto child = findArmatureRootNode(*node.mChildren[i], bones);
        if (child)
            return child;
    }

    return nullptr;
}
}  // namespace WBLoader
}  // namespace GameEngine
