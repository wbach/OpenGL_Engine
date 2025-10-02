#pragma once
#include "GameEngine/Components/BaseComponent.h"

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
    void Reload() override;

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
