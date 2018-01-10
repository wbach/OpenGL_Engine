#pragma once
#include "Mutex.hpp"

namespace GameEngine
{
struct SAplicationContext
{
	std::mutex renderingMutex;
};
}