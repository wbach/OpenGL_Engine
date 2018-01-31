#include "SceneFactoryBase.h"
#include "Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
	ScenePtr SceneFactoryBase::Create(const std::string& sceneName)
	{
		if (!IsExist(sceneName))
		{
			Log("SceneFactory::Create scene : " + sceneName + " not found.");
			return nullptr;
		}

		return scenesMap_[sceneName]();
	}
	ScenePtr SceneFactoryBase::Create(uint32 id)
	{
		if (!IsExist(id))
		{
			Log("SceneFactory::Create scene id: " + std::to_string(id) + " not found.");
			return nullptr;
		}

		return scenesMap_[GetSceneName(id)]();
	}
	void SceneFactoryBase::AddScene(const std::string& sceneName, CreateFunction func)
	{
		auto currentId = scenesMap_.size();

		idMap_[sceneName]	  = currentId;
		orderMap_[currentId]  = sceneName;
		scenesMap_[sceneName] = func;
	}
	const std::string& SceneFactoryBase::GetSceneName(uint32 id)
	{
		return orderMap_[id];
	}
	uint32 SceneFactoryBase::GetSceneId(const std::string& name)
	{
		return idMap_[name];
	}
	bool SceneFactoryBase::IsExist(uint32 id) const
	{
		return orderMap_.count(id) > 0;
	}
	bool SceneFactoryBase::IsExist(const std::string & name) const
	{
		return idMap_.count(name) > 0;
	}
}