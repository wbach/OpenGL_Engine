#include "Context.h"
#include "../../UtilsNetwork/Messages/RemoveCharacter/DisconnectCharacterMsg.h"

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
	std::lock_guard<std::mutex> lk(usersMutex);
	auto usageCharacter = users_[id].GetUsageCharacterId();

	if (usageCharacter)
		manager_.RemoveHero(id);

	users_.erase(id);

	for (const auto& user : users_)
		sendMessage_(user.first, std::make_unique<Network::DisconnectCharacterMsg>(usageCharacter.value()).get());
}

const UsersMap & Context::GetUsers()
{
	std::lock_guard<std::mutex> lk(usersMutex);
	return users_;
}

} // GameServer
