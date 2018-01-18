#include "BaseHero.h"

void GameServer::Hero::BaseHero::Handle(const Network::BoxMessage & message)
{
	for (auto& c : controllers_)
		c->Handle(message);
}
