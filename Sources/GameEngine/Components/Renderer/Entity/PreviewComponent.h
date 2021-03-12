#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectConstants.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/File.h"

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
