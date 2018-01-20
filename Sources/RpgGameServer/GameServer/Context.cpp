#include "Context.h"

namespace GameServer
{

void Context::NewUser(const std::string& name, uint32 id)
{
	std::lock_guard<std::mutex> lk(usersMutex);
	users_[id] = User(name, id);
	users_[id].SetCharacters(databaseWrapper_->GetCharacterByUser(id));
}

const UsersMap & Context::GetUsers()
{
	std::lock_guard<std::mutex> lk(usersMutex);
	return users_;
}

} // GameServer
