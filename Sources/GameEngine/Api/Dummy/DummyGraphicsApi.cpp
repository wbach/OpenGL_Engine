#include "DummyGraphicsApi.h"

GraphicsApi::ID GameEngine::DummyGraphicsApi::CreateParticle()
{
	return ++id;
}
