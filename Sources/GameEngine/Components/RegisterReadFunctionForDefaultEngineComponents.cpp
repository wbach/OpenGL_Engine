#include "RegisterReadFunctionForDefaultEngineComponents.h"

#include "Animation/Animator.h"
#include "Animation/BowPoseUpdater.h"
#include "Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/Enemy.h"
#include "Characters/Player.h"
#include "Controllers/CharacterController/ArrowController.h"
#include "Controllers/CharacterController/CharacterController.h"
#include "Controllers/EnemyController.h"
#include "Input/PlayerInputController.h"
#include "Physics/BoxShape.h"
#include "Physics/CapsuleShape.h"
#include "Physics/CylinderShape.h"
#include "Physics/MeshShape.h"
#include "Physics/Rigidbody.h"
#include "Physics/SphereShape.h"
#include "Physics/Terrain/TerrainShape.h"
#include "Renderer/Entity/RendererComponent.hpp"
#include "Renderer/Grass/GrassRendererComponent.h"
#include "Renderer/Particles/ParticleEffectComponent.h"
#include "Renderer/SkyBox/SkyBoxComponent.h"
#include "Renderer/Terrain/TerrainRendererComponent.h"
#include "Renderer/Trees/TreeRendererComponent.h"
#include "Renderer/Water/WaterRendererComponent.h"
#include "UnknownExternalComponent.h"

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
    registerReadFunction<UnknownExternalComponent>();
    registerReadFunction<Animator>();
    registerReadFunction<BowPoseUpdater>();
    registerReadFunction<CameraComponent>();
    registerReadFunction<ThridPersonCameraComponent>();
    registerReadFunction<Enemy>();
    registerReadFunction<Player>();
    registerReadFunction<CharacterController>();
    registerReadFunction<ArrowController>();
    registerReadFunction<EnemyController>();
    registerReadFunction<PlayerInputController>();
    registerReadFunction<BoxShape>();
    registerReadFunction<CylinderShape>();
    registerReadFunction<CapsuleShape>();
    registerReadFunction<MeshShape>();
    registerReadFunction<Rigidbody>();
    registerReadFunction<SphereShape>();
    registerReadFunction<TerrainShape>();
    registerReadFunction<RendererComponent>();
    registerReadFunction<GrassRendererComponent>();
    registerReadFunction<ParticleEffectComponent>();
    registerReadFunction<SkyBoxComponent>();
    registerReadFunction<TerrainRendererComponent>();
    registerReadFunction<TreeRendererComponent>();
    registerReadFunction<WaterRendererComponent>();
}
}  // namespace Components
}  // namespace GameEngine
