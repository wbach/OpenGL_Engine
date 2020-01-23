#include "Context.h"
#include "Common/Messages/RemoveCharacter/DisconnectCharacterMsg.h"

namespace GameServer
{
void Context::NewUser(const std::string& name, uint32 id)
{
    std::lock_guard<std::mutex> lk(usersMutex);
    users_[id] = User(name, id);
    users_[id].SetCharacters(databaseWrapper_->GetCharacterByUser(id));
}

void Context::DeleteUser(uint32 id)
{
    DEBUG_LOG("");

    std::lock_guard<std::mutex> lk(usersMutex);
    auto usageCharacter = users_[id].GetUsageCharacterId();

    if (usageCharacter)
        manager_.RemoveHero(usageCharacter.value());

    users_.erase(id);

    for (const auto& user : users_)
        sendMessage_(user.first, std::make_unique<common::DisconnectCharacterMsg>(usageCharacter.value()).get());
}

const UsersMap& Context::GetUsers()
{
    std::lock_guard<std::mutex> lk(usersMutex);
    return users_;
}

}  // namespace GameServer
