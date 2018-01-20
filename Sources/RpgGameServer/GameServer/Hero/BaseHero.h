#pragma once
#include "CharacetrContext.h"
#include "../../common/Controllers/IController.h"
#include <memory>
#include <unordered_map>

namespace GameServer
{
	namespace Hero
	{
		class BaseHero
		{
		public:
			BaseHero() {}
			BaseHero(const std::string& modelName, const std::string& name)
				: modelName_(modelName)
				, name_(name)
			{}
			virtual ~BaseHero() {}
			const std::string& GetName() { return name_; }
			void UpdateAllControllers(float dt);
			void AddController(Controllers::IControllerPtr cotroller);
			Controllers::IControllerPtr GetControllerByType(Controllers::Types type);
			//virtual void UseAbility(int id) = 0;
			//virtual void UseBaseAttack() = 0;
		public:
			CharacterContext context_;
			
		protected:
			std::string modelName_;
			std::string name_;
			std::unordered_map<Controllers::Types, Controllers::IControllerPtr> controllers_;
		};

		typedef std::shared_ptr<Hero::BaseHero> BaseHeroPtr;
	} // Hero
} // GameServer
