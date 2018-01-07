#pragma once
#include "../GameEngine/Input/InputManager.h"
#include "../GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "CharacterStats.h"
#include <list>

namespace CharacterActions
{
	enum Type
	{
		IDLE = 0,
		RUN,
		WALK,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
		SPELL_1,
		SPELL_2,
		SPELL_3,
		SPELL_4,
		COUNT
	};
}

class CPlayer : public CEntity
{
public:
    CPlayer(CInputManager* input_manager, CResourceManager& manager, const glm::vec3& normalized_scale, const std::string& filename);
	
    CharacterActions::Type GetAction() { return action; }
	void SetAction(CharacterActions::Type a);
	void SetPosition(const glm::vec3& p);


	void Update(float deltaTime);
	void ProcessState(CharacterActions::Type type);

	void Move(const float& delta_time);
	void Jump();
	void CheckInputs();
private:
    CInputManager* inputManager;
    CharacterActions::Type action = CharacterActions::IDLE;
    SCharacterStats characterStats;

    bool isGrounded = false;
	float upwardsSpeed = 0.f;

	std::list<CharacterActions::Type> states;
};
