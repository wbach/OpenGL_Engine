#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
class Mesh;

namespace Components
{
class PreviewComponent : public BaseComponent
{
public:
    PreviewComponent(ComponentContext&, GameObject&);

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void addModel(const File&, const File&);
    inline ModelWrapper& GetModelWrapper();
    const File& getOutputFile() const;

private:
    void Subscribe();
    void UnSubscribe();

private:
    ModelWrapper model_;
    File outputFile_;
    bool isSubscribed_;

private:
    std::unordered_map<std::string, LevelOfDetail> filenames_;
};

ModelWrapper& PreviewComponent::GetModelWrapper()
{
    return model_;
}
}  // namespace Components
}  // namespace GameEngine
