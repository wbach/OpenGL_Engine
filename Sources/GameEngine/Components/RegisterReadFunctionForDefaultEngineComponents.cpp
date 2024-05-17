#include "RegisterReadFunctionForDefaultEngineComponents.h"

#include "Animation/Animator.h"
#include "Animation/BowPoseUpdater.h"
#include "Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "Characters/Enemy.h"
#include "Characters/Player.h"
#include "Controllers/CharacterController/CharacterController.h"
#include "Controllers/EnemyController.h"
#include "Controllers/CharacterController/ArrowController.h"
#include "Input/PlayerInputController.h"
#include "Physics/BoxShape.h"
#include "Physics/CapsuleShape.h"
#include "Physics/MeshShape.h"
#include "Physics/Rigidbody.h"
#include "Physics/SphereShape.h"
#include "Physics/Terrain/TerrainShape.h"
#include "Renderer/Entity/RendererComponent.hpp"
#include "Renderer/Grass/GrassRendererComponent.h"
#include "Renderer/Particles/ParticleEffectComponent.h"
#include "Renderer/SkyBox/SkyBoxComponent.h"
#include "Renderer/Skydome/SkydomeComponent.h"
#include "Renderer/Terrain/TerrainRendererComponent.h"
#include "Renderer/Trees/TreeRendererComponent.h"
#include "Renderer/Water/WaterRendererComponent.h"

namespace GameEngine
{
namespace Components
{
template <typename T>
void registerReadFunction()
{
    T::registerReadFunctions();
}
void RegisterReadFunctionForDefaultEngineComponents()
{
    registerReadFunction<Animator>();
    registerReadFunction<BowPoseUpdater>();
    registerReadFunction<ThridPersonCameraComponent>();
    registerReadFunction<Enemy>();
    registerReadFunction<Player>();
    registerReadFunction<CharacterController>();
    registerReadFunction<ArrowController>();
    registerReadFunction<EnemyController>();
    registerReadFunction<PlayerInputController>();
    registerReadFunction<BoxShape>();
    registerReadFunction<CapsuleShape>();
    registerReadFunction<MeshShape>();
    registerReadFunction<Rigidbody>();
    registerReadFunction<SphereShape>();
    registerReadFunction<TerrainShape>();
    registerReadFunction<RendererComponent>();
    registerReadFunction<GrassRendererComponent>();
    registerReadFunction<ParticleEffectComponent>();
    registerReadFunction<SkyBoxComponent>();
    registerReadFunction<SkydomeComponent>();
    registerReadFunction<TerrainRendererComponent>();
    registerReadFunction<TreeRendererComponent>();
    registerReadFunction<WaterRendererComponent>();
}
}  // namespace Components
}  // namespace GameEngine
