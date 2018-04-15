#include "DummyGraphicsApi.h"

uint32 GameEngine::DummyGraphicsApi::CreateParticle()
{
	return ++id;
}
