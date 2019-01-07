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
    SkyBoxComponent();
    ~SkyBoxComponent();
    void SetDayTexture(const std::vector<std::string>& filenames);
    void SetNightTexture(const std::vector<std::string>& filenames);
    void SetModel(const std::string& filename);
    virtual void ReqisterFunctions() override;
    Texture* GetDayTexture();
    Texture* GetNightTexture();
    Model* GetModel();

private:
    void Subscribe();
    void UnSubscribe();

private:
    Texture* dayTexture_;
    Texture* nightTexture_;
    Model* model_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
