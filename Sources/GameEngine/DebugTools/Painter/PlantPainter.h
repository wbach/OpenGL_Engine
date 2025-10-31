#pragma once
#include <memory>

#include "GameEngine/Components/Renderer/Grass/GrassRendererComponent.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Painter.h"

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
        //FreeMeshTerrain,
        MeshTerrain,
        Terrain,
        Mesh
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

    PlantPainter(Dependencies&&, const File&, std::unique_ptr<IBrush>, PaintMode, float, float);
    ~PlantPainter() override;

    void Paint(const DeltaTime&) override;

private:
    File plantTexture;
    Dependencies dependencies;
    PaintMode mode;
    std::unique_ptr<IBrush> brush;
    float density;
    float randomness;
    Components::GrassRendererComponent* plantComponent;
};
}  // namespace GameEngine
