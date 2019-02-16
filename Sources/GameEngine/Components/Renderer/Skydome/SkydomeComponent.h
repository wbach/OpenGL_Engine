#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
class Texture;
class Model;

namespace Components
{
class SkydomeComponent : public BaseComponent
{
public:
    SkydomeComponent(const ComponentContext& componentContext, GameObject& gameObject);
    ~SkydomeComponent();

    virtual void ReqisterFunctions() override;
    Model* GetModel();

private:
    void LoadAndSubscribe();
    void UnSubscribe();

private:
    Model* model_;
    std::string modelFileName_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
