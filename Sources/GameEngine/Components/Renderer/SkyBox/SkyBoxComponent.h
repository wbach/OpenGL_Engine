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
    SkyBoxComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    SkyBoxComponent& SetDayTexture(const std::array<File, 6>&);
    SkyBoxComponent& SetNightTexture(const std::array<File, 6>&);
    SkyBoxComponent& SetModel(const std::string& filename);
    Texture* GetDayTexture();
    Texture* GetNightTexture();
    Model* GetModel();
    const std::string& GetModelFileName() const;

private:
    void DeleteTexture(CubeMapTexture*&);
    void Subscribe();
    void UnSubscribe();

private:
    CubeMapTexture* dayTexture_;
    CubeMapTexture* nightTexture_;
    Model* model_;
    std::array<File, 6> dayTextureFiles_;
    std::array<File, 6> nightTextureFiles_;
    std::string modelFileName_;
    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
