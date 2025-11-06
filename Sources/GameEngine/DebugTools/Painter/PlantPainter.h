#pragma once
#include <memory>
#include <optional>

#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Painter.h"
#include "Types.h"

namespace Utils
{
class Image;
}
namespace Input
{
class InputManager;
}

namespace GameEngine
{
class CameraWrapper;
class Projection;
class IBrush;

namespace Components
{
class ComponentController;
}  // namespace Components

class PlantPainter : public Painter
{
public:
    enum class PaintMode
    {
        // FreeMeshTerrain,
        //  MeshTerrain,
        Terrain  //,
                 // Mesh
    };

    struct Dependencies
    {
        Input::InputManager& inputManager;
        Utils::Thread::IThreadSync& threadSync;
        const CameraWrapper& camera;
        const Projection& projection;
        const Components::ComponentController& componentController;
        GameEngine::Scene& scene;
    };

    PlantPainter(Dependencies&&, const File&, std::unique_ptr<IBrush>, PaintMode, const Color&, const vec3&, float, float, float);
    ~PlantPainter() override;

    void Paint(const DeltaTime&) override;
    void Generate(const File&);
    void Generate(const std::optional<IdType>&);

private:
    void GenerateOnTerrain(Components::TerrainRendererComponent*);
    Components::GrassRendererComponent* getPaintedPlantComponent(GameObject&);

private:
    File plantTexture;
    Dependencies dependencies;
    PaintMode mode;
    std::unique_ptr<IBrush> brush;
    Color baseColor;
    vec3 colorRandomness;
    float sizeRandomness;
    float density;
    float randomness;
};
}  // namespace GameEngine
