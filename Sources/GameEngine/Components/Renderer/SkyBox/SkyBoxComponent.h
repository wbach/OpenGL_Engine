#pragma once
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
    ~SkyBoxComponent();
    SkyBoxComponent& SetDayTexture(const std::vector<std::string>& filenames);
    SkyBoxComponent& SetNightTexture(const std::vector<std::string>& filenames);
    SkyBoxComponent& SetModel(const std::string& filename);
    virtual void ReqisterFunctions() override;
    Texture* GetDayTexture();
    Texture* GetNightTexture();
    Model* GetModel();
    const std::vector<std::string>& GetDayTextureFiles() const;
    const std::vector<std::string>& GetNightTextureFiles() const;
    const std::string& GetModelFileName() const;

private:
    void Subscribe();
    void UnSubscribe();

private:
    Texture* dayTexture_;
    Texture* nightTexture_;
    Model* model_;
    std::vector<std::string> dayTextureFiles_;
    std::vector<std::string> nightTextureFiles_;
    std::string modelFileName_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
