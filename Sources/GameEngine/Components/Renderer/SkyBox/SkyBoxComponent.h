#pragma once
#include <array>
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
class Texture;
class Model;

namespace Components
{
class SkyBoxComponent : public BaseComponent
{
public:
    SkyBoxComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~SkyBoxComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;

    SkyBoxComponent& SetDayTexture(const std::array<std::string, 6>&);
    SkyBoxComponent& SetNightTexture(const std::array<std::string, 6>&);
    SkyBoxComponent& SetModel(const std::string& filename);
    Texture* GetDayTexture();
    Texture* GetNightTexture();
    Model* GetModel();
    const std::array<std::string, 6>& GetDayTextureFiles() const;
    const std::array<std::string, 6>& GetNightTextureFiles() const;
    const std::string& GetModelFileName() const;

private:
    void DeleteTexture(Texture*&);
    void Subscribe();
    void UnSubscribe();

private:
    Texture* dayTexture_;
    Texture* nightTexture_;
    Model* model_;
    std::array<std::string, 6> dayTextureFiles_;
    std::array<std::string, 6> nightTextureFiles_;
    std::string modelFileName_;
    bool isSubscribed_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
