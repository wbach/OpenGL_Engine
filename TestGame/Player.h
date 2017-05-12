#pragma once
#include "../GameEngine/Input/InputManager.h"
#include "../GameEngine/Objects/RenderAble/Entity.h"
#include "CharacterStats.h"

namespace CharacterActions
{
	enum Type {
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
	CPlayer(CInputManager* input_manager, CResourceManager& manager, const glm::vec3& normalized_scale, const std::string& filename)
		: m_InputManager(input_manager)
		, CEntity(manager, normalized_scale, filename)
	{}
	
	CharacterActions::Type GetAction() { return m_Action; }
	void SetAction(CharacterActions::Type action);
	void SetPosition(const glm::vec3& position);

	void Move(const float& delta_time);
	void Jump();
	void CheckInputs();
private:
	CInputManager* m_InputManager;
	CharacterActions::Type m_Action = CharacterActions::IDLE;
	SCharacterStats m_CharacterStats;

	bool m_IsGrounded = false;
	float upwardsSpeed = 0.f;
};