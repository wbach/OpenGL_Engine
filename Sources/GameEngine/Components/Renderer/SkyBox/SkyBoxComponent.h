#pragma once
#include <array>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
class CubeMapTexture;
class Texture;
class Model;

namespace Components
{
class SkyBoxComponent : public BaseComponent
{
public:
    std::vector<File> dayTextureFiles;
    std::vector<File> nightTextureFiles;
    File modelFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_CONST_VECTOR_OF_TEXTURES(dayTextureFiles)
        FIELD_CONST_VECTOR_OF_TEXTURES(nightTextureFiles)
        FIELD_FILE(modelFile)
    END_FIELDS()
    // clang-format on
public:
    SkyBoxComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    SkyBoxComponent& SetDayTexture(const std::vector<File>&);
    SkyBoxComponent& SetNightTexture(const std::vector<File>&);
    SkyBoxComponent& SetDayTexture(const std::array<File, 6>&);
    SkyBoxComponent& SetNightTexture(const std::array<File, 6>&);
    SkyBoxComponent& SetModel(const std::string& filename);
    Texture* GetDayTexture();
    Texture* GetNightTexture();
    Model* GetModel();
    const std::string& GetModelFileName() const;

private:
    void DeleteTexture(CubeMapTexture*&);
    void Init();
    void Subscribe();
    void UnSubscribe();
    void LoadTextures();

private:
    CubeMapTexture* dayTexture_;
    CubeMapTexture* nightTexture_;
    Model* model_;
    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
