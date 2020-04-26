#include "NetworkCharacterManger.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "ModelsCreator.h"
#include "TestGame/MRpg/MrpgGameContext.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"

namespace MmmoRpg
{
NetworkCharacterManager::NetworkCharacterManager(ModelsCreator* modelCreator,
                                                 GameEngine::Renderer::RenderersManager& rendererManager,
                                                 MrpgGameContext& gameContext, AddObject addObject, CreateObject createObject)
    : modelCreator_(modelCreator)
    , rendererManager_(rendererManager)
    , gameContext_(gameContext)
    , addObject_(addObject)
    , createObject_(createObject)
{
}

void NetworkCharacterManager::AddCharacter(uint32 id, uint32 classId, const vec3& position, const vec3& rotation,
                                           const common::Hero::CommonStats& stats)
{
    if (networkCharacters_.count(id) > 0)
        return;


    auto object = createObject_();
    auto& rendererComponent = object->AddComponent<GameEngine::Components::RendererComponent>();
    modelCreator_->CreateHero(classId, rendererComponent.GetModelWrapper());

    object->GetTransform().SetPositionAndRotation(position, DegreesVec3(rotation));

    networkCharacters_.insert({ id, std::make_shared<NetworkCharacter>(id, stats, *object) });
    addObject_(std::move(object));

    if (id == gameContext_.selectedCharacterId.first)
    {
        gameContext_.selectedCharacterId.second = SelectedCharacterState::READY_TO_USE;

        for (auto& s : onPlayerSubscribers_)
            s(networkCharacters_.at(id).get());
    }

}
void NetworkCharacterManager::Update(float deltaTime)
{
    for (auto& character : networkCharacters_)
    {
        character.second->UpdateControllers(deltaTime);
    }
}
void NetworkCharacterManager::RemoveCharacter(uint32 id)
{
    if (networkCharacters_.count(id) == 0)
        return;
    rendererManager_.UnSubscribe(&networkCharacters_.at(id)->GetGameObject());
    networkCharacters_.erase(id);
}
NetworkCharacter* NetworkCharacterManager::GetCharacter(uint32 id)
{
    if (networkCharacters_.count(id) == 0)
        return nullptr;

    return networkCharacters_.at(id).get();
}
void NetworkCharacterManager::SubscribeOnGetPlayer(OnGetPlayer f)
{
    onPlayerSubscribers_.push_back(f);
}
}  // MmmoRpg
