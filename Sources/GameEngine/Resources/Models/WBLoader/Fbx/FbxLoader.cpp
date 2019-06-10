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
struct MaterialProperty
{
    vec3 color_;
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
        if (importer_->Import(scene_))
        {
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

        importer_->Destroy();
        importer_ = nullptr;
    }

    void LoadTextures()
    {
        const int texture_count = scene_->GetTextureCount();
        for (int texture_index = 0; texture_index < texture_count; ++texture_index)
        {
            FbxTexture* texture         = scene_->GetTexture(texture_index);
            FbxFileTexture* textureFile = FbxCast<FbxFileTexture>(texture);
            if (textureFile && not textureFile->GetUserDataPtr())
            {
                try
                {
                    std::string fileName(textureFile->GetFileName());
                    auto texture = textureLoader_.LoadTexture(fileName);
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
            if (material && not material->GetUserDataPtr())
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
                if (mesh && not mesh->GetUserDataPtr())
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

    void InitializeMesh(const FbxMesh& mesh)
    {
        if (not mesh.GetNode())
            return;

        /* m_BonesInfo.push_back(SBonesInfo());
         SBonesInfo& bones_info = m_BonesInfo.back();*/

        // Count the polygon count of each material
        FbxLayerElementArrayTemplate<int>* materialIndice{nullptr};
        FbxGeometryElement::EMappingMode materialMappingMode{FbxGeometryElement::eNone};
        if (mesh.GetElementMaterial())
        {
            materialIndice      = &mesh.GetElementMaterial()->GetIndexArray();
            materialMappingMode = mesh.GetElementMaterial()->GetMappingMode();
        }

        FbxStringList uvNames;
        mesh.GetUVSetNames(uvNames);
        const char* uvName{nullptr};

        if (uvNames.GetCount())
        {
            uvName = uvNames[0];
        }

        objects_.back().meshes.emplace_back();
        auto& newMesh = objects_.back().meshes.back();

        newMesh.vertexBuffer.reserve(3 * mesh.GetControlPointsCount());
        for (int x = 0; x < mesh.GetControlPointsCount(); ++x)
        {
            const auto& currentVertex = mesh.GetControlPoints()[x];

            newMesh.vertexBuffer.emplace_back();
            newMesh.vertexBuffer.back().position =
                vec3{static_cast<float>(currentVertex[0]), static_cast<float>(currentVertex[1]),
                     static_cast<float>(currentVertex[2])};
        }

        int vertexIndex = 0;
        for (int polygonIndex = 0; polygonIndex < mesh.GetPolygonCount(); ++polygonIndex)
        {
            if (materialIndice)  // && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
            {
                newMesh.material = materials_[materialIndice->GetAt(polygonIndex)];
            }

            for (int verticeIndex = 0; verticeIndex < 3; ++verticeIndex)
            {
                const int controlPointIndex = mesh.GetPolygonVertex(polygonIndex, verticeIndex);
                const auto& currentVertex   = mesh.GetControlPoints()[controlPointIndex];

                newMesh.vertexBuffer[vertexIndex].indexes = controlPointIndex;

                FbxVector4 currentNormal;
                mesh.GetPolygonVertexNormal(polygonIndex, verticeIndex, currentNormal);
                auto& normal = newMesh.vertexBuffer[3 * controlPointIndex].normal;
                normal.x     = static_cast<float>(currentNormal[0]);
                normal.y     = static_cast<float>(currentNormal[1]);
                normal.z     = static_cast<float>(currentNormal[2]);

                bool unmappedUV{false};
                FbxVector2 currentUV;
                mesh.GetPolygonVertexUV(polygonIndex, verticeIndex, uvName, currentUV, unmappedUV);

                newMesh.vertexBuffer[2 * controlPointIndex].uvs.x = static_cast<float>(currentUV[0]);
                newMesh.vertexBuffer[2 * controlPointIndex].uvs.y = static_cast<float>(currentUV[1]);

                ++vertexIndex;
            }
        }
    }
};

FbxLoader::FbxLoader(GameEngine::ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
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
