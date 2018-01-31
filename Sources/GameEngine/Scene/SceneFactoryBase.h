#pragma once
#include "Types.h"
#include <memory>
#include <functional>
#include <unordered_map>

class CScene;

namespace GameEngine
{
	typedef std::unique_ptr<CScene> ScenePtr;
	typedef std::function<ScenePtr()> CreateFunction;
	typedef std::unordered_map<std::string, CreateFunction> ScenesMap;
	typedef std::unordered_map<uint32, std::string> OrderMap;
	typedef std::unordered_map<std::string, uint32> IdMap;

	class SceneFactoryBase
	{
	public:
		ScenePtr Create(const std::string&);
		ScenePtr Create(uint32);

		virtual ~SceneFactoryBase() {}

		uint32 ScenesSize() {return scenesMap_.size();}
		const std::string& GetSceneName(uint32 id);
		uint32 GetSceneId(const std::string& name);

		bool IsExist(uint32 name) const;
		bool IsExist(const std::string& name) const;
	
	protected:
		void AddScene(const std::string&, CreateFunction);

	private:
		ScenesMap scenesMap_;
		OrderMap orderMap_;
		IdMap idMap_;
	};	

	typedef std::shared_ptr<SceneFactoryBase> SceneFactoryBasePtr;
}