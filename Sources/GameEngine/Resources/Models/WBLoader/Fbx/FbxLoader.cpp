#include "FbxLoader.h"
#include <Logger/Log.h>
#include <fbxsdk.h>
#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
{
namespace WBLoader
{
struct FbxLoader::Pimpl
{
    FbxManager* manager_;

    FbxScene* scene_;
    FbxTime frameTime_;
    FbxImporter* importer_;
    FbxAnimLayer* currentAnimLayer_;
    FbxArray<FbxString*> animStackNameArray_;
    int fileFormat{-1};

    ITextureLoader& textureLoader_;

    Pimpl(ITextureLoader& textureLoader)
        : textureLoader_(textureLoader)
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
        if (not manager_->GetIOPluginRegistry()->DetectReaderFileFormat(filename.c_str(), fileFormat))
        {
            // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
            fileFormat = manager_->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
        }

        // Initialize the importer by providing a filename.
        if (not importer_->Initialize(filename.c_str(), fileFormat))
        {
            DEBUG_LOG("Initialize importer error.");
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
                // InitializeMaterial(material);
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
                    // InitializeMesh(mesh);
                }
            }
        }

        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            LoadCacheRecursive(node->GetChild(i));
        }
    }

    std::pair<vec3, Texture*> GetMaterialProperty(const FbxSurfaceMaterial& material, const char* propertyName, const char* factorPropertyName)
    {
        std::pair<vec3, Texture*> result;

        const FbxProperty property       = material.FindProperty(propertyName);
        const FbxProperty factorProperty = material.FindProperty(factorPropertyName);

        if (property.IsValid() and factorProperty.IsValid())
        {
            auto color         = property.Get<FbxDouble3>();
            double colorFactor = factorProperty.Get<FbxDouble>();

            result.first.x = static_cast<float>(color[0] * colorFactor);
            result.first.y = static_cast<float>(color[1] * colorFactor);
            result.first.z = static_cast<float>(color[2] * colorFactor);
        }

        if (property.IsValid())
        {
            const int textureCount = property.GetSrcObjectCount<FbxFileTexture>();
            if (textureCount)
            {
                const FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>();
                if (texture and texture->GetUserDataPtr())
                {
                    result.second = (static_cast<Texture*>(texture->GetUserDataPtr()));
                }
            }
        }

        return result;
    }
};

FbxLoader::FbxLoader(GameEngine::ITextureLoader& textureLoader)
    : AbstractLoader(textureLoader.GetGraphicsApi(), textureLoader)
{
}

void FbxLoader::ParseFile(const std::string& filename)
{
    impl_->ReadFile(filename);
}

bool FbxLoader::CheckExtension(const std::string& filename)
{
    auto ext = Utils::GetFileExtension(filename);
    return ext == "fbx" || ext == "FBX" || ext == "Fbx";
}

}  // namespace WBLoader
}  // namespace GameEngine
