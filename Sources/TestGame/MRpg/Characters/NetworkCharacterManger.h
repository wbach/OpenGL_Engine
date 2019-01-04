#pragma once
#include <functional>
#include <memory>
#include "NetworkCharacter.h"

namespace common
{
namespace Hero
{
struct CommonStats;
}  // Hero
}  // common

namespace GameEngine
{
namespace Renderer
{
class RenderersManager;
}  // Renderer
}  // GameEngine

namespace MmmoRpg
{
class ModelsCreator;
struct MrpgGameContext;
typedef std::function<void(NetworkCharacter*)> OnGetPlayer;

class NetworkCharacterManager
{
    typedef std::function<void(std::unique_ptr<GameEngine::GameObject>&)> AddObject;

public:
    NetworkCharacterManager(ModelsCreator* modelCreator, GameEngine::Renderer::RenderersManager& rendererManager,
                            MrpgGameContext& gameContext, AddObject addObject);
    void AddCharacter(uint32 id, uint32 classId, const vec3& position, const vec3& rotation,
                      const common::Hero::CommonStats& stats);
    void Update(float deltaTime);
    void RemoveCharacter(uint32 id);
    NetworkCharacter* GetCharacter(uint32 id);
    void SubscribeOnGetPlayer(OnGetPlayer);

private:
    std::list<OnGetPlayer> onPlayerSubscribers_;
    std::unordered_map<uint32, std::shared_ptr<NetworkCharacter>> networkCharacters_;
    ModelsCreator* modelCreator_;
    GameEngine::Renderer::RenderersManager& rendererManager_;
    MrpgGameContext& gameContext_;
    AddObject addObject_;
};

typedef std::unique_ptr<NetworkCharacterManager> NetworkCharacterManagerPtr;
}  // MmmoRpg
