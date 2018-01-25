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
			BaseHero()
			: BaseHero("","") 
			{}
			BaseHero(const std::string& modelName, const std::string& name)
				: class_(0)
				, modelName_(modelName)
				, name_(name)
			{}
			virtual ~BaseHero() {}

		public:
			//virtual void UseAbility(int id) = 0;
			//virtual void UseBaseAttack() = 0;
			
			void AddController(Controllers::IControllerPtr cotroller);
			
			uint8 GetClass() {return class_;}
			const std::string& GetName() { return name_; }
			Controllers::IControllerPtr GetControllerByType(Controllers::Types type);
			
			void UpdateAllControllers(float dt);
			
		public:
			CharacterContext context_;
			
		protected:
			uint8 class_;
			std::string modelName_;
			std::string name_;
			std::unordered_map<Controllers::Types, Controllers::IControllerPtr> controllers_;
		};

		typedef std::shared_ptr<Hero::BaseHero> BaseHeroPtr;
	} // Hero
} // GameServer
