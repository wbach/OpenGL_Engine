#include "ShaderFactory.h"
#include "GameEngine/Renderers/GUI/Text/FontShader.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureShader.h"
#include "GameEngine/Renderers/Objects/Entity/Shaders/DefferedEntityShader.h"
#include "GameEngine/Renderers/Objects/Entity/Shaders/EntityShader.h"
#include "GameEngine/Renderers/Objects/Grass/Shaders/GrassShader.h"
#include "GameEngine/Renderers/Objects/Particles/Shaders/AnimatedParticlesShader.h"
#include "GameEngine/Renderers/Objects/Particles/Shaders/ParticlesShader.h"
#include "GameEngine/Renderers/Objects/Plants/Shaders/PlantsShader.h"
#include "GameEngine/Renderers/Objects/Shadows/Shaders/ShadowShader.h"
#include "GameEngine/Renderers/Objects/SkyBox/Shaders/SkyBoxShader.h"
#include "GameEngine/Renderers/Objects/Skydome/Shaders/SkydomeShader.h"
#include "GameEngine/Renderers/Objects/Terrain/Mesh/Shaders/TerrainMeshShader.h"
#include "GameEngine/Renderers/Objects/Terrain/Shaders/TerrainNormalMapShader.h"
#include "GameEngine/Renderers/Objects/Terrain/Shaders/TerrainShader.h"
#include "GameEngine/Renderers/Objects/Tree/Shaders/TreeShader.h"
#include "GameEngine/Renderers/Objects/Water/Shaders/WaterShader.h"
#include "GameEngine/Renderers/Postproccesing/Blur/Shaders/BlurShader.h"
#include "GameEngine/Renderers/Postproccesing/ColorFliper/Shaders/ColorFliperShader.h"
#include "GameEngine/Renderers/Postproccesing/DefferedLighting/Shaders/DefferedShader.h"
#include "GameEngine/Renderers/Postproccesing/SSAO/Shaders/SSAOShader.h"
#include "GameEngine/Shaders/Loading/LoadingShader.h"
#include "GameEngine/Renderers/DebugElements/Shaders/GridShader.h"

namespace GameEngine
{
ShaderFactory::ShaderFactory(GraphicsApi::IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
{
}
std::unique_ptr<IShaderProgram> ShaderFactory::create(GraphicsApi::Shaders type)
{
    switch (type)
    {
        case GraphicsApi::Shaders::Loading:
            return std::make_unique<LoadingShader>(graphicsApi_);
        case GraphicsApi::Shaders::Entity:
            return std::make_unique<DefferedEntityShader>(graphicsApi_);
        case GraphicsApi::Shaders::Grid:
            return std::make_unique<GridShader>(graphicsApi_);
        case GraphicsApi::Shaders::Grass:
            return std::make_unique<GrassShader>(graphicsApi_);
        case GraphicsApi::Shaders::Particles:
            return std::make_unique<ParticlesShader>(graphicsApi_);
        case GraphicsApi::Shaders::AnimatedParticles:
            return std::make_unique<AnimatedParticlesShader>(graphicsApi_);
        case GraphicsApi::Shaders::Plants:
            return std::make_unique<PlantsShader>(graphicsApi_);
        case GraphicsApi::Shaders::Shadows:
            return std::make_unique<ShadowShader>(graphicsApi_);
        case GraphicsApi::Shaders::SkyBox:
            return std::make_unique<SkyBoxShader>(graphicsApi_);
        case GraphicsApi::Shaders::Skydome:
            return std::make_unique<SkydomeShader>(graphicsApi_);
        case GraphicsApi::Shaders::Terrain:
            return std::make_unique<TerrainShader>(graphicsApi_);
        case GraphicsApi::Shaders::TerrainNormal:
            return std::make_unique<TerrainNormalMapShader>(graphicsApi_);
        case GraphicsApi::Shaders::TerrainMesh:
            return std::make_unique<TerrainMeshShader>(graphicsApi_);
        case GraphicsApi::Shaders::Tree:
            return std::make_unique<TreeShader>(graphicsApi_);
        case GraphicsApi::Shaders::Water:
            return std::make_unique<WaterShader>(graphicsApi_);
        case GraphicsApi::Shaders::Blur:
            return std::make_unique<BlurShader>(graphicsApi_);
        case GraphicsApi::Shaders::ColorFilper:
            return std::make_unique<ColorFliperShader>(graphicsApi_);
        case GraphicsApi::Shaders::Deffered:
            return std::make_unique<DefferedShader>(graphicsApi_);
        case GraphicsApi::Shaders::SSAO:
            return std::make_unique<SSAOShader>(graphicsApi_);
        case GraphicsApi::Shaders::Font:
            return std::make_unique<FontShader>(graphicsApi_);
        case GraphicsApi::Shaders::Texture:
            return std::make_unique<Renderer::Gui::GuiTextureShader>(graphicsApi_);
    }
    return std::unique_ptr<IShaderProgram>();
}
}  // namespace GameEngine
