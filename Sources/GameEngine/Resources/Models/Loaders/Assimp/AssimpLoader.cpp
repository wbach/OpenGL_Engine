#include "AssimpLoader.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>
#include <XML/XMLUtils.h>
#include <XML/XmlReader.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/Models/MeshData.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "magic_enum/magic_enum.hpp"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <assimp/Importer.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <span>

#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
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

struct SimplifiedTransform
{
    std::optional<vec3> position;
    std::optional<Quaternion> rotation;
    std::optional<vec3> scale;
};
struct TmpKeyFrame
{
    float timeStamp{0.f};
    std::unordered_map<Animation::JointId, SimplifiedTransform> transforms;
};

struct TmpAnimationClip
{
    std::vector<TmpKeyFrame> frames;

    TmpKeyFrame& getTmpKeyFrame(float time)
    {
        auto iter =
            std::find_if(frames.begin(), frames.end(), [time](const auto& frame) { return compare(frame.timeStamp, time); });

        if (iter != frames.end())
        {
            return *iter;
        }

        TmpKeyFrame frame;
        frame.timeStamp = time;
        auto it         = std::lower_bound(frames.begin(), frames.end(), frame,
                                           [](const auto& a, const auto& b) { return a.timeStamp < b.timeStamp; });

        return *frames.insert(it, frame);
    }

    void addNotSimplifiedFramesToClip(Animation::AnimationClip& clip)
    {
        Animation::KeyFrame const* previousFrame{nullptr};
        for (const auto& tmpFrame : frames)
        {
            Animation::KeyFrame keyFrame;
            keyFrame.timeStamp = tmpFrame.timeStamp;

            for (const auto& [jointId, simplifiedTransform] : tmpFrame.transforms)
            {
                auto& jointTransform = keyFrame.transforms[jointId];

                if (simplifiedTransform.position)
                {
                    jointTransform.position = *simplifiedTransform.position;
                }
                else
                {
                    if (previousFrame != nullptr)
                    {
                        auto iter = previousFrame->transforms.find(jointId);
                        if (iter != previousFrame->transforms.end())
                        {
                            jointTransform.position = iter->second.position;
                        }
                    }
                }

                if (simplifiedTransform.rotation)
                {
                    jointTransform.rotation = *simplifiedTransform.rotation;
                }
                else
                {
                    if (previousFrame != nullptr)
                    {
                        auto iter = previousFrame->transforms.find(jointId);
                        if (iter != previousFrame->transforms.end())
                        {
                            jointTransform.rotation = iter->second.rotation;
                        }
                    }
                }

                if (simplifiedTransform.scale)
                {
                    jointTransform.scale = *simplifiedTransform.scale;
                }
                else
                {
                    if (previousFrame != nullptr)
                    {
                        auto iter = previousFrame->transforms.find(jointId);
                        if (iter != previousFrame->transforms.end())
                        {
                            jointTransform.scale = iter->second.scale;
                        }
                    }
                }
            }
            previousFrame = &keyFrame;
            clip.AddFrame(keyFrame);
        }
    }
};

}  // namespace
AssimpLoader::AssimpLoader(ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
    auto version = std::to_string(aiGetVersionMajor()) + "." + std::to_string(aiGetVersionMinor());
    LOG_DEBUG << "Assimp version : " << version;
}

AssimpLoader::~AssimpLoader()
{
}

bool AssimpLoader::ParseFile(const File& file)
{
    LOG_DEBUG << file.GetAbsolutePath();

    // to do create factory and loader per one loading
    bones_.clear();
    objects.clear();
    boneIdPool_.reset();

    uint32 flags = aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

    if (loadingParameters_.meshOptimize == MeshOptimize::optimized)
    {
        flags |= aiProcess_JoinIdenticalVertices | aiProcess_OptimizeGraph |
                 aiProcess_OptimizeMeshes;  // aiProcess_RemoveRedundantMaterials | aiProcess_SplitLargeMeshes
    }

    Assimp::Importer importer;
    auto scene = importer.ReadFile(file.GetAbsolutePath().string().c_str(), flags);

    if (not scene)
    {
        LOG_ERROR << "import file error : " << importer.GetErrorString();
        return false;
    }

    if (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_DEBUG << "import scene incomplete";
    }

    currentProcessingFile_ = file;

    auto globalInverseTransform = scene->mRootNode->mTransformation;
    globalInverseTransform.Inverse();

    objects.push_back(Object());
    object_ = &objects.back();

    readAdditionInfoFile(file);
    // printTree(*scene->mRootNode);
    recursiveProcess(*scene, *scene->mRootNode);
    processSkeleton(*scene);
    processAnimations(*scene);
    importer.FreeScene();

    currentProcessingFile_.reset();

    LOG_DEBUG << "Done. " << file.GetAbsolutePath();

    return true;
}

bool AssimpLoader::CheckExtension(const File& file)
{
    return file.IsFormat({"AMF", "3DS",      "AC",      "ASE", "ASSBIN", "B3D",  "BVH",   "COLLADA", "DXF", "CSM",
                          "DAE", "HMP",      "IRRMESH", "IRR", "LWO",    "LWS",  "MD2",   "MD3",     "MD5", "MD5MESH",
                          "MDC", "MDL",      "NFF",     "NDO", "OFF",    "OBJ",  "OGRE",  "OPENGEX", "PLY", "MS3D",
                          "COB", "BLEND",    "IFC",     "XGL", "FBX",    "Q3D",  "Q3BSP", "RAW",     "SIB", "SMD",
                          "STL", "TERRAGEN", "3D",      "X",   "X3D",    "GLTF", "3MF",   "MMD",     "STEP", "GLB"});

    // AMF 3DS AC ASE ASSBIN B3D BVH COLLADA DXF CSM HMP IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL NFF NDO OFF OBJ
    // OGRE OPENGEX PLY MS3D COB BLEND IFC XGL FBX Q3D Q3BSP RAW SIB SMD STL TERRAGEN 3D X X3D GLTF 3MF MMD STEP
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
    object_->meshes.emplace_back();
    auto& newMesh        = object_->meshes.back();
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
void AssimpLoader::loadBonesData(const aiScene& scene, const aiMesh& mesh, LoaderMesh& newMesh)
{
    if (mesh.mNumBones == 0)
    {
        return;
    }

    if (not not mesh.mBones)
    {
        LOG_WARN << "Num of bonse  > 0 but no bones data. mNumBones = " << mesh.mNumBones;
    }

    if (not newMesh.meshRawData_)
    {
        LOG_ERROR << "Mesh data not set!";
        return;
    }
    auto& meshData = *newMesh.meshRawData_;

    std::vector<std::vector<JointInfo>> jointInfos;
    jointInfos.resize(mesh.mNumVertices);
    auto vertexCount = mesh.mNumVertices * 4;
    meshData.joinIds_.resize(vertexCount);
    meshData.bonesWeights_.resize(vertexCount);

    LOG_DEBUG << "mesh.mNumBones: " << mesh.mNumBones;
    for (const auto& bone : std::span(mesh.mBones, mesh.mNumBones))
    {
        std::string name(bone->mName.data);
        uint32 boneId = 0;

        auto iter = bones_.find(name);
        if (iter == bones_.end())
        {
            boneId = boneIdPool_.nextId();

            BoneInfo info;
            info.id     = boneId;
            info.matrix = convert(bone->mOffsetMatrix);
            bones_.insert({name, info});
        }
        else
        {
            auto& boneInfo  = iter->second;
            boneId          = boneInfo.id;
            boneInfo.matrix = convert(bone->mOffsetMatrix);
        }

        if (bone->mNumWeights > 0 and not bone->mWeights)
        {
            LOG_WARN << "bone.mWeights is nullptr, but mNumWeights > 0. mNumWeights: " << bone->mNumWeights;
            continue;
        }

        for (uint32 j = 0; j < bone->mNumWeights; j++)
        {
            const auto& mWeights = bone->mWeights[j];
            auto vertexId        = mWeights.mVertexId;
            auto weight          = mWeights.mWeight;
            jointInfos[vertexId].push_back({boneId, weight});
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
            LOG_WARN << "To many bones per vertex : " << currentProcessingFile_;
        }
        ++i;
    }
}
void AssimpLoader::processSkeleton(const aiScene& scene)
{
    if (not bones_.empty())
    {
        const aiNode& node = *scene.mRootNode;

        LOG_DEBUG << "Rootnode bone name: " << node.mName.data << " : " << currentProcessingFile_;

        for (const auto& bone : bones_)
            LOG_DEBUG << bone.first;

        auto armatureNode = findArmatureRootNode(node);
        if (armatureNode)
        {
            LOG_DEBUG << "Root node found : " << armatureNode->mName.data;
            object_->skeleton_ = Animation::Joint{};
            createSkeleton(*armatureNode, *object_->skeleton_);
        }
    }
}
void AssimpLoader::createSkeleton(const aiNode& node, Animation::Joint& rootJoint)
{
    std::string name(node.mName.data);
    Animation::Joint child;

    if (bones_.count(name))
    {
        auto& info       = bones_.at(name);
        rootJoint.id     = info.id;
        rootJoint.offset = info.matrix;
    }
    else
    {
        rootJoint.id = boneIdPool_.nextId();
    }

    rootJoint.name      = name;
    rootJoint.transform = convert(node.mTransformation);

    for (uint32 i = 0; i < node.mNumChildren; ++i)
    {
        Animation::Joint child;
        createSkeleton(*node.mChildren[i], child);
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

        LOG_DEBUG << "Animation : " << name;
        objects.back().animationClips_.insert({name, processAnimation(aiAnim)});
    }
}

std::filesystem::path getTexturePath(const std::optional<File>& currentProcessingFile, const std::string& path)
{
    if (std::filesystem::exists(path))
        return path;

    if (not currentProcessingFile)
        return path;

    LOG_DEBUG << "Texture not found : " << path
              << ". Searching recursively in model based directory : " << currentProcessingFile->GetAbsolutePath().parent_path();

    auto parentDirPathTexture =
        Utils::FindFile(File(path).GetFilename(), currentProcessingFile->GetAbsolutePath().parent_path()).string();
    if (not parentDirPathTexture.empty())
        return parentDirPathTexture;

    return Utils::FindFile(File(path).GetFilename(), EngineLocalConf.files.getDataPath()).string();
}

GeneralTexture* CreateMaterialTexture(const std::optional<File>& currentProcessingFile, const aiMaterial& material,
                                      ITextureLoader& loader, aiTextureType type)
{
    GeneralTexture* texture{nullptr};
    auto count = material.GetTextureCount(type);
    if (count > 1)
    {
        LOG_DEBUG << "Multiple the same type textures found in one material. Not supported. " << magic_enum::enum_name(type)
                  << " count: " << count;
    }
    for (uint32 i = 0; i < material.GetTextureCount(type); ++i)
    {
        aiString path;
        material.GetTexture(type, i, &path);
        TextureParameters parameters;
        parameters.filter = GraphicsApi::TextureFilter::LINEAR;
        parameters.mimap = GraphicsApi::TextureMipmap::LINEAR;
        const auto& file = getTexturePath(currentProcessingFile, path.C_Str());
        texture          = loader.LoadTexture(file, parameters);
    }
    return texture;
}

Material AssimpLoader::processMaterial(const aiScene& scene, const aiMesh& mesh) const
{
    if (!scene.HasMaterials())
        return Material();

    aiMaterial* mat = scene.mMaterials[mesh.mMaterialIndex];
    Material material;

    aiString name;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_NAME, name))
    {
        material.name = std::string(name.C_Str());
    }

    aiColor4D diff;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff))
    {
        material.baseColor = glm::vec4(diff.r, diff.g, diff.b, 1.0f);
    }

    float metal = 0.0f;
    float rough = 0.7f;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, diff))
    {
        rough = 1.0f - glm::clamp(diff.r, 0.0f, 1.0f);
    }
    material.roughnessFactor = rough;
    material.metallicFactor  = metal;

    float opacity = 1.0f;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_OPACITY, opacity))
    {
        material.opacityCutoff = 0.5f;
        if (opacity < 1.0f)
        {
            material.flags |= MAT_ALPHA_TEST;
        }
    }

    material.baseColorTexture = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_DIFFUSE);
    material.normalTexture    = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_NORMALS);
    material.metallicTexture  = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_SPECULAR);
    material.roughnessTexture = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_SHININESS);
    material.ambientOcclusionTexture = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_AMBIENT);
    material.opacityTexture          = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_OPACITY);
    material.displacementTexture     = CreateMaterialTexture(currentProcessingFile_, *mat, textureLoader_, aiTextureType_HEIGHT);

    return material;
}

Animation::AnimationClip AssimpLoader::processAnimation(const aiAnimation& aiAnim)
{
    TmpAnimationClip tmpClip;

    for (uint32 i = 0; i < aiAnim.mNumChannels; i++)
    {
        const auto& animChannel = *aiAnim.mChannels[i];
        std::string jointName   = animChannel.mNodeName.data;
        auto joint              = object_->skeleton_->getJoint(jointName);

        if (not joint)
        {
            LOG_DEBUG << "Joint not found in skeleton : " << jointName;
            continue;
        }

        for (uint32 i = 0; i < animChannel.mNumRotationKeys; i++)
        {
            const auto& rotationKey = animChannel.mRotationKeys[i];
            auto& frame             = tmpClip.getTmpKeyFrame(static_cast<float>(rotationKey.mTime / aiAnim.mTicksPerSecond));
            frame.transforms[joint->id].rotation = covnertQuat(rotationKey.mValue);
        }

        for (uint32 i = 0; i < animChannel.mNumPositionKeys; i++)
        {
            const auto& positionKey = animChannel.mPositionKeys[i];
            auto& frame             = tmpClip.getTmpKeyFrame(static_cast<float>(positionKey.mTime / aiAnim.mTicksPerSecond));
            frame.transforms[joint->id].position = covnertVec3(positionKey.mValue);
        }

        for (uint32 i = 0; i < animChannel.mNumScalingKeys; i++)
        {
            const auto& scalingKey = animChannel.mScalingKeys[i];
            auto& frame            = tmpClip.getTmpKeyFrame(static_cast<float>(scalingKey.mTime / aiAnim.mTicksPerSecond));
            frame.transforms[joint->id].scale = covnertVec3(scalingKey.mValue);
        }
    }

    Animation::AnimationClip clip(aiAnim.mName.data);
    clip.SetLength(static_cast<float>(aiAnim.mDuration / aiAnim.mTicksPerSecond));
    tmpClip.addNotSimplifiedFramesToClip(clip);

    return clip;
}

Animation::KeyFrame& AssimpLoader::getFrameByTimeStamp(Animation::AnimationClip& clip, float time)
{
    auto frame = clip.getFrame(Animation::TimeStamp{time});

    if (not frame)
    {
        Animation::KeyFrame frame;
        frame.timeStamp = time;
        return clip.AddFrame(frame);
    }
    else
    {
        return *frame;
    }
}

void AssimpLoader::readAdditionInfoFile(const File& file)
{
    auto additionInfoFile = file.CreateFileWithExtension("xml");
    if (additionInfoFile.exist())
    {
        Utils::XmlReader xmlReader;
        if (xmlReader.Read(additionInfoFile.GetAbsolutePath().string()))
        {
            auto correction = xmlReader.Get("correction");
            if (correction)
            {
                vec3 position(0.f);
                vec3 rotation(0.f);
                vec3 scale(1.f);

                ::Read(xmlReader.Get("position", correction), position);
                ::Read(xmlReader.Get("rotation", correction), rotation);
                ::Read(xmlReader.Get("scale", correction), scale);

                object_->transformMatrix = Utils::CreateTransformationMatrix(position, DegreesVec3(rotation), scale);
            }
        }
    }
}

void AssimpLoader::printTree(const aiNode& node, uint32 depth) const
{
    std::string name(node.mName.data);
    std::string str;

    for (uint32 i = 0; i < depth; ++i)
        str += "---";

    /* LOG TO FIX*/ LOG_ERROR << (str + name + "( Depth : " + std::to_string(depth) + ")");

    for (uint32 i = 0; i < node.mNumChildren; ++i)
    {
        printTree(*node.mChildren[i], depth + 1);
    }
}

const aiNode* AssimpLoader::findArmatureRootNode(const aiNode& node)
{
    std::string name(node.mName.data);

    if (bones_.count(name))
        return &node;

    for (uint32 i = 0; i < node.mNumChildren; ++i)
    {
        auto child = findArmatureRootNode(*node.mChildren[i]);
        if (child)
        {
            for (uint32 j = i; j < node.mNumChildren; ++j)
            {
                std::string childName(node.mChildren[i]->mName.data);
                if (bones_.count(childName))
                {
                    return &node;
                }
            }
            return child;
        }
    }

    return nullptr;
}
std::vector<std::filesystem::path> AssimpLoader::getAllTexturesFilesFromModel(const std::filesystem::path& modelPath)
{
    std::set<std::filesystem::path> uniquePaths;
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(modelPath.string(), 0);

    if (!scene || !scene->HasMaterials())
    {
        return {};
    }

    const aiTextureType types[] = {aiTextureType_DIFFUSE,
                                   aiTextureType_SPECULAR,
                                   aiTextureType_AMBIENT,
                                   aiTextureType_EMISSIVE,
                                   aiTextureType_HEIGHT,
                                   aiTextureType_NORMALS,
                                   aiTextureType_SHININESS,
                                   aiTextureType_OPACITY,
                                   aiTextureType_DISPLACEMENT,
                                   aiTextureType_LIGHTMAP,
                                   aiTextureType_REFLECTION,
                                   aiTextureType_BASE_COLOR,
                                   aiTextureType_NORMAL_CAMERA,
                                   aiTextureType_EMISSION_COLOR,
                                   aiTextureType_METALNESS,
                                   aiTextureType_DIFFUSE_ROUGHNESS,
                                   aiTextureType_AMBIENT_OCCLUSION,
                                   aiTextureType_UNKNOWN};

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material = scene->mMaterials[i];

        for (aiTextureType type : types)
        {
            unsigned int texCount = material->GetTextureCount(type);
            for (unsigned int j = 0; j < texCount; ++j)
            {
                aiString str;
                if (material->GetTexture(type, j, &str) == AI_SUCCESS)
                {
                    std::string pathStr = str.C_Str();
                    if (!pathStr.empty() && pathStr[0] != '*')
                    {
                        uniquePaths.insert(std::filesystem::path(pathStr));
                    }
                }
            }
        }
    }

    return {uniquePaths.begin(), uniquePaths.end()};
}
}  // namespace GameEngine
