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
class ICamera;
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
        //,
        Terrain,
        Erase
    };

    struct Dependencies
    {
        Input::InputManager& inputManager;
        Utils::Thread::IThreadSync& threadSync;
        const ICamera& camera;
        const Components::ComponentController& componentController;
        GameEngine::Scene& scene;
    };

    PlantPainter(Dependencies&&, std::unique_ptr<IBrush>);
    PlantPainter(Dependencies&&, const File&, std::unique_ptr<IBrush>, PaintMode, const Color&, const vec3&, float, float, float);
    ~PlantPainter() override;

    void Paint(const DeltaTime&) override;
    void Generate(const File&);
    void Generate(const std::optional<IdType>&);

private:
    void Paint(const vec2&);
    void Erase(const vec2&);
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
