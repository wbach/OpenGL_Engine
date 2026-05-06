#include "RegisterReadFunctionForDefaultEngineComponents.h"

#include "Animation/Animator.h"
#include "Animation/PoseUpdater.h"
#include "Audio/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "Characters/Enemy.h"
#include "Characters/Player.h"
#include "Controllers/AIController.h"
#include "Controllers/CharacterController/ArrowController.h"
#include "Controllers/CharacterController/CharacterController.h"
#include "Controllers/EnemyController.h"
#include "Dialogue/DialogueComponent.h"
#include "Gameplay/Attack/MeleeAttackComponent.h"
#include "Gameplay/Attack/WeaponComponent.h"
#include "Gameplay/CharacterStats/CharacterStatsComponent.h"
#include "Gameplay/HealthComponent.h"
#include "Gameplay/Inventory/CombatStatsComponent.h"
#include "Gameplay/Inventory/ConsumableComponent.h"
#include "Gameplay/Inventory/EquipmentComponent.h"
#include "Gameplay/Inventory/EquippableComponent.h"
#include "Gameplay/Inventory/InventoryComponent.h"
#include "Gameplay/Inventory/ItemIdentityComponent.h"
#include "Gameplay/Inventory/ItemVisualComponent.h"
#include "Gameplay/QuestViewer/QuestViewerComponent.h"
#include "Input/PlayerInputController.h"
#include "Lights/DirectionalLightComponent.h"
#include "Lights/PointLightComponent.h"
#include "Lights/SpotLightComponent.h"
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
    registerReadFunction<AudioComponent>();
    registerReadFunction<Animator>();
    registerReadFunction<AIController>();
    registerReadFunction<PoseUpdater>();
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
    registerReadFunction<DirectionalLightComponent>();
    registerReadFunction<PointLightComponent>();
    registerReadFunction<SpotLightComponent>();
    registerReadFunction<DialogueComponent>();
    registerReadFunction<QuestViewerComponent>();
    registerReadFunction<InventoryComponent>();
    registerReadFunction<ItemIdentityComponent>();
    registerReadFunction<EquippableComponent>();
    registerReadFunction<CombatStatsComponent>();
    registerReadFunction<ConsumableComponent>();
    registerReadFunction<ItemVisualComponent>();
    registerReadFunction<EquipmentComponent>();
    registerReadFunction<WeaponComponent>();
    registerReadFunction<MeleeAttackComponent>();
    registerReadFunction<HealthComponent>();
    registerReadFunction<CharacterStatsComponent>();
}
}  // namespace Components
}  // namespace GameEngine
