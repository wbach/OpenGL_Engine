#include "FbxLoader.h"
#include <Logger/Log.h>
#include <fbxsdk.h>
#include <vector>
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Models/Material.h"

namespace GameEngine
{
namespace WBLoader
{
namespace
{
const char* clusterModes[] = {"Normalize", "Additive", "Total1"};
std::unordered_map<std::string, mat4*> boneMatrixes;
}  // namespace
vec3 convert(const FbxVector4& v)
{
    vec3 result;
    result.x = static_cast<float>(v[0]);
    result.y = static_cast<float>(v[1]);
    result.z = static_cast<float>(v[2]);
    return result;
}

vec2 convert(const FbxVector2& v)
{
    vec2 result;
    result.x = static_cast<float>(v[0]);
    result.y = static_cast<float>(v[1]);
    return result;
}

glm::mat4 convert(FbxAMatrix& mx)
{
    glm::mat4 m(1.f);
    m[0][0] = static_cast<float>(mx[0][0]);
    m[0][1] = static_cast<float>(mx[0][1]);
    m[0][2] = static_cast<float>(mx[0][2]);
    m[0][3] = static_cast<float>(mx[0][3]);
    m[1][0] = static_cast<float>(mx[1][0]);
    m[1][1] = static_cast<float>(mx[1][1]);
    m[1][2] = static_cast<float>(mx[1][2]);
    m[1][3] = static_cast<float>(mx[1][3]);
    m[2][0] = static_cast<float>(mx[2][0]);
    m[2][1] = static_cast<float>(mx[2][1]);
    m[2][2] = static_cast<float>(mx[2][2]);
    m[2][3] = static_cast<float>(mx[2][3]);
    m[3][0] = static_cast<float>(mx[3][0]);
    m[3][1] = static_cast<float>(mx[3][1]);
    m[3][2] = static_cast<float>(mx[3][2]);
    m[3][3] = static_cast<float>(mx[3][3]);
    return m;
}

Animation::Joint* findJointByName(Animation::Joint& rootJoint, const std::string& name)
{
    if (rootJoint.name == name)
        return &rootJoint;

    for (auto& child : rootJoint.children)
    {
        auto result = findJointByName(child, name);
        if (result)
            return result;
    }
    return nullptr;
}

void createSkeleton(Animation::Joint& joint, FbxNode* node)
{
    if (not node)
        return;

    if (boneMatrixes.count(node->GetName()) > 0)
    {
        DEBUG_LOG("Duplicate bone name found. Ignore it. " + node->GetName());
        return;
    }

    joint.name = node->GetName();
    joint.size = 1;

    boneMatrixes.insert({joint.name, &joint.transform});

    for (auto childIndex = 0; childIndex < node->GetChildCount(); ++childIndex)
    {
        Animation::Joint child;
        createSkeleton(child, node->GetChild(childIndex));
        joint.children.push_back(child);
        joint.size += child.size;
    }
}

struct MaterialProperty
{
    vec3 color_;
    char unused0[4];
    GeneralTexture* texture_{nullptr};
};
struct FbxLoader::Pimpl
{
    FbxManager* manager_;

    FbxScene* scene_;
    FbxTime frameTime_;
    FbxImporter* importer_;
    FbxAnimLayer* currentAnimLayer_;
    FbxArray<FbxString*> animStackNameArray_;
    int fileFormat{-1};

    std::vector<Material> materials_;

    ITextureLoader& textureLoader_;
    std::list<WBLoader::Object>& objects_;

    Pimpl(ITextureLoader& textureLoader, std::list<WBLoader::Object>& objects)
        : textureLoader_(textureLoader)
        , objects_(objects)
    {
        Init();
        CreateFbxScene();
    }

    ~Pimpl()
    {
        //        scene_->Destroy();
        //        currentAnimLayer_->Destroy();
        FbxArrayDelete(animStackNameArray_);
        importer_->Destroy();
        manager_->Destroy();
    }
    void Init()
    {
        manager_ = FbxManager::Create();

        if (not manager_)
        {
            ERROR_LOG("unable to create FBX manager.");
        }
        else
        {
            INFO_LOG("Autodesk FBX SDK version :" + std::string(manager_->GetVersion()));
        }

        // Create an IOSettings object. This object holds all import/export settings.
        FbxIOSettings* ios = FbxIOSettings::Create(manager_, IOSROOT);
        manager_->SetIOSettings(ios);

        // Load plugins from the executable directory (optional)
        FbxString path = FbxGetApplicationDirectory();
        manager_->LoadPluginsDirectory(path.Buffer());
        importer_ = FbxImporter::Create(manager_, "");
    }

    void CreateFbxScene()
    {
        // Create an FBX scene. This object holds most objects imported/exported from/to files.
        scene_ = FbxScene::Create(manager_, "MyScene");
        if (not scene_)
        {
            ERROR_LOG("Unable to create FBX scene");
        }
    }

    void ReadFile(const File& file)
    {
        DEBUG_LOG("Read fullpath : " + file.GetAbsoultePath());

        if (not manager_->GetIOPluginRegistry()->DetectReaderFileFormat(file.GetAbsoultePath().c_str(), fileFormat))
        {
            // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
            fileFormat = manager_->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
        }

        // Initialize the importer by providing a filename.
        if (not importer_->Initialize(file.GetAbsoultePath().c_str(), fileFormat))
        {
            DEBUG_LOG("Initialize importer error : " + file.GetFilename());
            return;
        }

        ImportScene();
    }

    void ImportScene()
    {
        if (not importer_->Import(scene_))
        {
            ERROR_LOG("can not import scene.");
            return;
        }

        FbxAxisSystem sceneAxisSystem = scene_->GetGlobalSettings().GetAxisSystem();
        FbxAxisSystem ourAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
        if (sceneAxisSystem != ourAxisSystem)
        {
            ourAxisSystem.ConvertScene(scene_);
        }

        FbxSystemUnit scene_system_unit = scene_->GetGlobalSettings().GetSystemUnit();
        if (scene_system_unit.GetScaleFactor() != 1.0)
        {
            FbxSystemUnit::cm.ConvertScene(scene_);
        }

        scene_->FillAnimStackNameArray(animStackNameArray_);

        for (int i = 0; i < animStackNameArray_.Size(); ++i)
        {
            DEBUG_LOG(" " + animStackNameArray_.GetAt(i)->Buffer());
        }

        // Get the list of all the cameras in the scene.
        // FillCameraArray(mScene, mCameraArray);

        FbxGeometryConverter geom_converter(manager_);
        if (not geom_converter.Triangulate(scene_, true))
        {
            ERROR_LOG("Triangulate mesh failed.");
            return;
        }

        if (not geom_converter.SplitMeshesPerMaterial(scene_, true))
        {
            WARNING_LOG("Split mesh per material failed.");
        }

        LoadTextures();

        objects_.push_back(WBLoader::Object());
        // auto &object = objects_.back();
        // object.name = "NewObject";

        LoadCacheRecursive(scene_->GetRootNode());
        frameTime_.SetTime(0, 0, 0, 1, 0, scene_->GetGlobalSettings().GetTimeMode());

        //        importer_->Destroy();
        //        importer_ = nullptr;
    }

    void LoadTextures()
    {
        const int texture_count = scene_->GetTextureCount();
        for (int texture_index = 0; texture_index < texture_count; ++texture_index)
        {
            FbxTexture* texture         = scene_->GetTexture(texture_index);
            FbxFileTexture* textureFile = FbxCast<FbxFileTexture>(texture);
            if (textureFile and not textureFile->GetUserDataPtr())
            {
                try
                {
                    std::string fileName(textureFile->GetFileName());

                    DEBUG_LOG("Convert file name path : " + fileName);

                    auto found = fileName.find("Textures");

                    if (found != std::string::npos)
                    {
                        fileName = fileName.substr(found);
                    }
                    DEBUG_LOG("New file name path : " + fileName);
                    TextureParameters params;
                    params.mimap = GraphicsApi::TextureMipmap::LINEAR;

                    auto texture = textureLoader_.LoadTexture(fileName, params);
                    if (texture)
                        textureFile->SetUserDataPtr(texture);
                }
                catch (const std::exception& e)
                {
                    ERROR_LOG("exception : " + e.what());
                    return;
                }
            }
        }
    }

    void LoadCacheRecursive(FbxNode* node, std::string t = "")
    {
        for (int i = 0; i < node->GetMaterialCount(); ++i)
        {
            FbxSurfaceMaterial* material = node->GetMaterial(i);
            if (material and not material->GetUserDataPtr())
            {
                materials_.push_back(InitializeMaterial(*material));
            }
        }

        for (int i = 0; i < node->GetNodeAttributeCount(); ++i)
        {
            FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);

            if (nodeAttribute and nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh* mesh = static_cast<FbxMesh*>(nodeAttribute);

                if (mesh and not mesh->GetUserDataPtr())
                {
                    InitializeMesh(*mesh);
                }
            }
        }

        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            LoadCacheRecursive(node->GetChild(i), t + "-");
        }
    }

    MaterialProperty GetMaterialProperty(const FbxSurfaceMaterial& material, const char* propertyName,
                                         const char* factorPropertyName)
    {
        MaterialProperty result;

        const FbxProperty property       = material.FindProperty(propertyName);
        const FbxProperty factorProperty = material.FindProperty(factorPropertyName);

        if (property.IsValid() and factorProperty.IsValid())
        {
            auto color         = property.Get<FbxDouble3>();
            double colorFactor = factorProperty.Get<FbxDouble>();

            result.color_.x = static_cast<float>(color[0] * colorFactor);
            result.color_.y = static_cast<float>(color[1] * colorFactor);
            result.color_.z = static_cast<float>(color[2] * colorFactor);
        }

        if (property.IsValid())
        {
            const int textureCount = property.GetSrcObjectCount<FbxFileTexture>();
            if (textureCount)
            {
                const FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>();
                if (texture and texture->GetUserDataPtr())
                {
                    result.texture_ = (static_cast<GeneralTexture*>(texture->GetUserDataPtr()));
                }
            }
        }

        return result;
    }

    Material InitializeMaterial(const FbxSurfaceMaterial& material)
    {
        Material result;
        result.name = material.GetName();

        auto ambient = GetMaterialProperty(material, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
        result.ambient        = ambient.color_;
        result.ambientTexture = ambient.texture_;

        auto diffuse = GetMaterialProperty(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
        result.diffuse        = diffuse.color_;
        result.diffuseTexture = diffuse.texture_;

        auto specular =
            GetMaterialProperty(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
        result.specular        = specular.color_;
        result.specularTexture = specular.texture_;

        FbxProperty shininess_property = material.FindProperty(FbxSurfaceMaterial::sShininess);
        if (shininess_property.IsValid())
        {
            double shininess   = shininess_property.Get<FbxDouble>();
            result.shineDamper = static_cast<float>(shininess);
        }

        return result;
    }

    FbxAMatrix GetGeometry(FbxNode* node)
    {
        const FbxVector4 lt = node->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lr = node->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 ls = node->GetGeometricScaling(FbxNode::eSourcePivot);

        return FbxAMatrix(lt, lr, ls);
    }

    void InitializeMesh(const FbxMesh& fbxMesh)
    {
        if (fbxMesh.GetPolygonCount() == 0)
            return;

        objects_.back().meshes.emplace_back();
        auto& newMesh = objects_.back().meshes.back();

        if (fbxMesh.GetElementMaterial())
        {
            auto materialIndice = &fbxMesh.GetElementMaterial()->GetIndexArray();
            newMesh.material    = materials_[static_cast<size_t>(materialIndice->GetAt(0))];
        }

        FbxStringList uvNames;
        fbxMesh.GetUVSetNames(uvNames);
        const char* uvName{nullptr};

        if (uvNames.GetCount())
        {
            uvName = uvNames[0];
        }

        newMesh.vertexBuffer.reserve(static_cast<size_t>(fbxMesh.GetControlPointsCount()));

        for (int polygonIndex = 0; polygonIndex < fbxMesh.GetPolygonCount(); ++polygonIndex)
        {
            for (int verticeIndex = 0; verticeIndex < 3; ++verticeIndex)
            {
                const int controlPointIndex       = fbxMesh.GetPolygonVertex(polygonIndex, verticeIndex);
                const auto& currentVertexPosition = fbxMesh.GetControlPoints()[controlPointIndex];

                FbxVector4 currentNormal;
                fbxMesh.GetPolygonVertexNormal(polygonIndex, verticeIndex, currentNormal);

                bool unmappedUV{false};
                FbxVector2 currentUV;
                fbxMesh.GetPolygonVertexUV(polygonIndex, verticeIndex, uvName, currentUV, unmappedUV);

                newMesh.vertexBuffer.emplace_back();
                auto& vertexBuffer    = newMesh.vertexBuffer.back();
                vertexBuffer.indexes  = controlPointIndex;
                vertexBuffer.position = convert(currentVertexPosition);
                vertexBuffer.normal   = convert(currentNormal);
                vertexBuffer.uvs      = convert(currentUV);
            }
        }

        InitializeSkeleton(fbxMesh, newMesh);
    }

    void InitializeSkeleton(const FbxMesh& fbxMesh, Mesh& newMesh)
    {
        int skinCount = fbxMesh.GetDeformerCount(FbxDeformer::eSkin);

        DEBUG_LOG("Skin count " + std::to_string(skinCount));

        if (skinCount > 1)
        {
            DEBUG_LOG("More than 1 skin detected. Not supported");
            return;
        }

        for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
        {
            std::optional<Animation::Joint> rootJoint;
            uint32 jointId = 1;

            auto skinDeformer = static_cast<FbxSkin*>(fbxMesh.GetDeformer(skinIndex, FbxDeformer::eSkin));

            DEBUG_LOG("Cluster count " + std::to_string(skinDeformer->GetClusterCount()));

            for (int clusterIndex = 0; clusterIndex < skinDeformer->GetClusterCount(); ++clusterIndex)
            {
                auto cluster     = skinDeformer->GetCluster(clusterIndex);
                auto clusterMode = cluster->GetLinkMode();
                auto link        = cluster->GetLink();
                if (not link)
                    continue;

                int indexCount  = cluster->GetControlPointIndicesCount();
                int* indices    = cluster->GetControlPointIndices();
                double* weights = cluster->GetControlPointWeights();

                for (int k = 0; k < indexCount; k++)
                {
                    JointInfo jointInfo;
                    jointInfo.weight = clusterMode != FbxCluster::eAdditive ? static_cast<float>(weights[k]) : 1.f;
                    jointInfo.id     = jointId;

                    auto iter = std::find_if(
                        newMesh.vertexBuffer.begin(), newMesh.vertexBuffer.end(),
                        [vertexBufferIndex = indices[k]](const auto& vb) { return vb.indexes == vertexBufferIndex; });

                    if (iter != newMesh.vertexBuffer.end())
                        iter->jointInfo.push_back(jointInfo);
                }
                FbxAMatrix transformMatrix;
                transformMatrix = cluster->GetTransformMatrix(transformMatrix);

                FbxAMatrix linkMatrix;
                linkMatrix = cluster->GetTransformLinkMatrix(linkMatrix);

                auto lReferenceGeometry = GetGeometry(fbxMesh.GetNode());
                transformMatrix *= lReferenceGeometry;

                auto fbxJointMatrix = linkMatrix.Inverse() * transformMatrix;
                // fbxJointMatrix      = fbxJointMatrix.Inverse().Transpose();

                auto linkName       = link->GetName();
                auto parentLinkName = link->GetParent() ? link->GetParent()->GetName() : "noParent";

                // fbxJointMatrix = fbxMesh.GetNode()->EvaluateGlobalTransform();

                //                auto lReferenceGlobalCurrentPosition = fbxMesh.GetNode()->EvaluateLocalTransform();
                //                fbxJointMatrix = lReferenceGlobalCurrentPosition.Inverse() * fbxJointMatrix;
                auto jointMatrix = convert(fbxJointMatrix);

                if (not rootJoint)
                {
                    rootJoint       = Animation::Joint();
                    rootJoint->name = parentLinkName;

                    Animation::Joint childJoint;
                    childJoint.name      = linkName;
                    childJoint.id        = jointId++;
                    childJoint.transform = jointMatrix;
                    rootJoint->addChild(std::move(childJoint));
                }
                else
                {
                    auto parentJoint = findJointByName(*rootJoint, parentLinkName);

                    if (parentJoint)
                    {
                        Animation::Joint childJoint;
                        childJoint.name      = linkName;
                        childJoint.id        = jointId++;
                        childJoint.transform = jointMatrix;
                        parentJoint->addChild(std::move(childJoint));
                    }
                    else
                    {
                        WARNING_LOG("parent not found. Parent name : " + parentLinkName + ", link name : " + linkName);
                    }
                }
            }

//            for (auto& vb : newMesh.vertexBuffer)
//            {
//                float totalWeight = 0.f;
//                for (auto ji : vb.jointInfo)
//                {
//                    totalWeight += ji.weight;
//                }
//                DEBUG_LOG("Joints per vertex :" + std::to_string(vb.jointInfo.size()));
//                DEBUG_LOG("Total weight : " + std::to_string(totalWeight));
//            }
            newMesh.skeleton_ = std::move(*rootJoint);
        }
    }
};

FbxLoader::FbxLoader(GameEngine::ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
}

FbxLoader::~FbxLoader()
{
}

void FbxLoader::ParseFile(const File& filename)
{
    impl_ = std::make_unique<Pimpl>(textureLoader_, objects);
    impl_->ReadFile(filename);
    impl_.reset();
}

bool FbxLoader::CheckExtension(const std::string& filename)
{
    auto ext = Utils::GetFileExtension(filename);
    return ext == "fbx" or ext == "FBX" or ext == "Fbx";
}

}  // namespace WBLoader
}  // namespace GameEngine
