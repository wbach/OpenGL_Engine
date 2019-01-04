#include "NetworkCharacterManger.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "ModelsCreator.h"
#include "TestGame/MRpg/MrpgGameContext.h"

namespace MmmoRpg
{
NetworkCharacterManager::NetworkCharacterManager(ModelsCreator* modelCreator,
                                                 GameEngine::Renderer::RenderersManager& rendererManager,
                                                 MrpgGameContext& gameContext, AddObject addObject)
    : modelCreator_(modelCreator)
    , rendererManager_(rendererManager)
    , gameContext_(gameContext)
    , addObject_(addObject)
{
}

void NetworkCharacterManager::AddCharacter(uint32 id, uint32 classId, const vec3& position, const vec3& rotation,
                                           const common::Hero::CommonStats& stats)
{
    if (networkCharacters_.count(id) > 0)
        return;

    auto modelWrapper                 = modelCreator_->CreateHero(classId);
    networkCharacters_[id]            = std::make_shared<NetworkCharacter>(id, stats, modelWrapper);
    auto entity                       = networkCharacters_[id]->GetEntity();
    entity->worldTransform.SetPosition(position);
    entity->worldTransform.SetRotation(rotation);
    entity->worldTransform.isDynamic_ = true;
    entity->worldTransform.TakeSnapShoot();
    rendererManager_.Subscribe(entity);
    addObject_(std::unique_ptr<GameEngine::GameObject>(entity));

    if (id == gameContext_.selectedCharacterId.first)
    {
        gameContext_.selectedCharacterId.second = SelectedCharacterState::READY_TO_USE;

        for (auto& s : onPlayerSubscribers_)
            s(networkCharacters_[id].get());
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
    rendererManager_.UnSubscribe(networkCharacters_[id]->GetEntity());
    networkCharacters_.erase(id);
}
NetworkCharacter* NetworkCharacterManager::GetCharacter(uint32 id)
{
    if (networkCharacters_.count(id) == 0)
        return nullptr;

    return networkCharacters_[id].get();
}
void NetworkCharacterManager::SubscribeOnGetPlayer(OnGetPlayer f)
{
    onPlayerSubscribers_.push_back(f);
}
}  // MmmoRpg
