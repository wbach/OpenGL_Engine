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
#include "GameEngine/Renderers/Objects/Terrain/Shaders/TerrainShader.h"
#include "GameEngine/Renderers/Objects/Tree/Shaders/TreeShader.h"
#include "GameEngine/Renderers/Objects/Water/Shaders/WaterShader.h"
#include "GameEngine/Renderers/Postproccesing/Blur/Shaders/BlurShader.h"
#include "GameEngine/Renderers/Postproccesing/ColorFliper/Shaders/ColorFliperShader.h"
#include "GameEngine/Renderers/Postproccesing/DefferedLighting/Shaders/DefferedShader.h"
#include "GameEngine/Renderers/Postproccesing/SSAO/Shaders/SSAOShader.h"
#include "GameEngine/Shaders/Loading/LoadingShader.h"

namespace GameEngine
{
ShaderFactory::ShaderFactory(IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
{
}
std::unique_ptr<IShaderProgram> ShaderFactory::create(Shaders type)
{
    switch (type)
    {
        case Shaders::Loading:
            return std::make_unique<LoadingShader>(graphicsApi_);
        case Shaders::Entity:
            return std::make_unique<DefferedEntityShader>(graphicsApi_);
        // case Shaders::DefferedEntity:
        //    return std::make_unique<DefferedEntityShader>(graphicsApi_);
        case Shaders::Grass:
            return std::make_unique<GrassShader>(graphicsApi_);
        case Shaders::Particles:
            return std::make_unique<ParticlesShader>(graphicsApi_);
        case Shaders::AnimatedParticles:
            return std::make_unique<AnimatedParticlesShader>(graphicsApi_);
        case Shaders::Plants:
            return std::make_unique<PlantsShader>(graphicsApi_);
        case Shaders::Shadows:
            return std::make_unique<ShadowShader>(graphicsApi_);
        case Shaders::SkyBox:
            return std::make_unique<SkyBoxShader>(graphicsApi_);
        case Shaders::Terrain:
            return std::make_unique<TerrainShader>(graphicsApi_);
        case Shaders::Tree:
            return std::make_unique<TreeShader>(graphicsApi_);
        case Shaders::Water:
            return std::make_unique<WaterShader>(graphicsApi_);
        case Shaders::Blur:
            return std::make_unique<BlurShader>(graphicsApi_);
        case Shaders::ColorFilper:
            return std::make_unique<ColorFliperShader>(graphicsApi_);
        case Shaders::Deffered:
            return std::make_unique<DefferedShader>(graphicsApi_);
        case Shaders::SSAO:
            return std::make_unique<SSAOShader>(graphicsApi_);
        case Shaders::Font:
            return std::make_unique<FontShader>(graphicsApi_);
        case Shaders::Texture:
            return std::make_unique<Renderer::Gui::GuiTextureShader>(graphicsApi_);
    }
    return std::unique_ptr<IShaderProgram>();
}
}  // namespace GameEngine
