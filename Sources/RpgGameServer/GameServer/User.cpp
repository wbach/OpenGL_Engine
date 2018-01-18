#include "User.h"
#include "Logger/Log.h"
#include "Database/IDatabaseWrapper.h"

namespace GameServer
{
	User::User(Context context, const std::string & name, uint32 id)
		: id_(id)
		, name_(name)
		, context_(context)

	{
		if (context_.databaseWrapper_ == nullptr)
		{
			Log("User::User databaseWrapper_ is nullptr.");
			return;
		}
		characters_ = context_.databaseWrapper_->GetCharacterByUser(id_);
	}
	uint32 User::GetId()
	{
		return id_;
	}
	uint32 User::GetUsageCharacterId()
	{
		return usageCharacterId_;
	}
	Network::IMessagePtr User::Handle(const Network::IMessagePtr)
	{
		return Network::IMessagePtr();
	}
} // GameServer