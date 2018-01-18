#pragma once
#include "Stats.h"
#include <memory>
#include "../../common/Controllers/IController.h"

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
			void Handle(const Network::BoxMessage& message);

			//virtual void UseAbility(int id) = 0;
			//virtual void UseBaseAttack() = 0;
			
		protected:
			std::string modelName_;
			std::string name_;
			Stats stats_;
			std::list<Controllers::IControllerPtr> controllers_;
		};

		typedef std::shared_ptr<Hero::BaseHero> BaseHeroPtr;
	} // Hero
} // GameServer
