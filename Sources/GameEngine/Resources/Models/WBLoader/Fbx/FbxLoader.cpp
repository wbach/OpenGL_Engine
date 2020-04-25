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
std::unordered_map<std::string, mat4*> boneMatrixes;
}
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
    Texture* texture_{nullptr};
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
            ERROR_LOG("Autodesk FBX SDK version :" + std::string(manager_->GetVersion()));
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

    void ReadFile(const std::string& filename)
    {
        auto fullpath = EngineConf_GetFullDataPath(filename);
        DEBUG_LOG("Read fullpath : " + fullpath);

        if (not manager_->GetIOPluginRegistry()->DetectReaderFileFormat(fullpath.c_str(), fileFormat))
        {
            // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
            fileFormat = manager_->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
        }

        // Initialize the importer by providing a filename.
        if (not importer_->Initialize(fullpath.c_str(), fileFormat))
        {
            DEBUG_LOG("Initialize importer error : " + filename);
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

        // Get the list of all the cameras in the scene.
        // FillCameraArray(mScene, mCameraArray);

        FbxGeometryConverter geom_converter(manager_);
        geom_converter.Triangulate(scene_, true, true);
        geom_converter.SplitMeshesPerMaterial(scene_, true);

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

                    auto texture = textureLoader_.LoadTexture(fileName, TextureParameters());
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

    void LoadCacheRecursive(FbxNode* node)
    {
        for (int i = 0; i < node->GetMaterialCount(); ++i)
        {
            FbxSurfaceMaterial* material = node->GetMaterial(i);
            if (material and not material->GetUserDataPtr())
            {
                materials_.push_back(InitializeMaterial(*material));
            }
        }

        FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
        if (nodeAttribute)
        {
            if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh* mesh = node->GetMesh();
                if (mesh and not mesh->GetUserDataPtr())
                {
                    InitializeMesh(*mesh);
                }
            }
        }

        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            LoadCacheRecursive(node->GetChild(i));
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
                    result.texture_ = (static_cast<Texture*>(texture->GetUserDataPtr()));
                }
            }
        }

        return result;
    }

    Material InitializeMaterial(const FbxSurfaceMaterial& material)
    {
        Material result;

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

    void InitializeMesh(const FbxMesh& fbxMesh)
    {
        if (not fbxMesh.GetNode())
            return;

        /* m_BonesInfo.push_back(SBonesInfo());
         SBonesInfo& bones_info = m_BonesInfo.back();*/

        // Count the polygon count of each material
        FbxLayerElementArrayTemplate<int>* materialIndice{nullptr};
        FbxGeometryElement::EMappingMode materialMappingMode{FbxGeometryElement::eNone};
        if (fbxMesh.GetElementMaterial())
        {
            materialIndice      = &fbxMesh.GetElementMaterial()->GetIndexArray();
            materialMappingMode = fbxMesh.GetElementMaterial()->GetMappingMode();
        }

        FbxStringList uvNames;
        fbxMesh.GetUVSetNames(uvNames);
        const char* uvName{nullptr};

        if (uvNames.GetCount())
        {
            uvName = uvNames[0];
        }

        objects_.back().meshes.emplace_back();
        auto& newMesh = objects_.back().meshes.back();

        newMesh.vertexBuffer.reserve(static_cast<size_t>(fbxMesh.GetControlPointsCount()));

        for (int polygonIndex = 0; polygonIndex < fbxMesh.GetPolygonCount(); ++polygonIndex)
        {
            if (materialIndice)
            {
                newMesh.material = materials_[static_cast<size_t>(materialIndice->GetAt(polygonIndex))];
            }

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

        return;
        // FbxCluster::ELinkMode clusterMode =
        //    ((FbxSkin*)fbxMesh.GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

        int skinCount = fbxMesh.GetDeformerCount(FbxDeformer::eSkin);

        // bones_info.bones.resize(lPolygonCount * 3);
        // int count              = 0;
        // unsigned int boneIndex = 0;
        DEBUG_LOG("Skin count " + std::to_string(skinCount));

        if (skinCount > 1)
        {
            DEBUG_LOG("More than 1 skin detected. Not supported");
        }

        for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
        {
            auto skinDeformer = static_cast<FbxSkin*>(fbxMesh.GetDeformer(skinIndex, FbxDeformer::eSkin));

            DEBUG_LOG("Cluster count " + std::to_string(skinDeformer->GetClusterCount()));

            auto cluster = skinDeformer->GetCluster(0);
            if (not cluster->GetLink())
                continue;

            auto link = cluster->GetLink();

            createSkeleton(newMesh.skeleton_, link);

            for (int clusterIndex = 0; clusterIndex < skinDeformer->GetClusterCount(); ++clusterIndex)
            {
                auto cluster = skinDeformer->GetCluster(clusterIndex);
                if (not cluster->GetLink())
                    continue;

                auto link = cluster->GetLink();

                if (boneMatrixes.count(link->GetName()) == 0)
                {
                    DEBUG_LOG("Bone not found : " + link->GetName());
                    continue;
                }

                FbxAMatrix linkMatrix;
                cluster->GetTransformLinkMatrix(linkMatrix);
                (*boneMatrixes.at(link->GetName())) = convert(linkMatrix);

                for (int k = 0; k < cluster->GetControlPointIndicesCount(); ++k)
                {
                    int index         = cluster->GetControlPointIndices()[k];
                    auto vertextCount = fbxMesh.GetPolygonCount() * 3;
                    if (index >= vertextCount)
                        continue;

                    auto weight = static_cast<float>(cluster->GetControlPointWeights()[k]);
                    newMesh.vertexBuffer[index].jointInfo.push_back(JointInfo{0, weight });

                    // DEBUG_LOG("Weight : " + std::to_string(weight));

                    // for (auto& v : newMesh.vertexBuffer)
                    //                   {
                    //                       if (v.indexes.x == index)
                    //                       {
                    //
                    //                       }
                    //                   }
                    //                   for (int x = 0; x < 100; ++x)
                    //                   {
                    //                       cluster->GetControlPointWeights()[x];
                    // std::cout << "x : " << x << " " << cluster->GetControlPointIndicesCount() << std::endl;
                    //}
                    // newMesh.vertexBuffer[index].weights
                }
            }
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

void FbxLoader::ParseFile(const std::string& filename)
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
