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
    SkydomeComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;

    Model* GetModel();

private:
    void LoadAndSubscribe();
    void UnSubscribe();

private:
    Model* model_;
    std::string modelFileName_;
    bool isSubscribed_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
