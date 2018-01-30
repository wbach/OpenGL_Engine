#pragma once
#include "Types.h"
#include <memory>

class CScene;

namespace GameEngine
{
	class ISceneFactory
	{
	public:
		virtual std::unique_ptr<CScene> Create(const std::string&) = 0;
		virtual ~ISceneFactory() {}
	};

	typedef std::shared_ptr<ISceneFactory> ISceneFactoryPtr;
}