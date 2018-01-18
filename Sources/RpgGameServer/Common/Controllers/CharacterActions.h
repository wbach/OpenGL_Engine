#pragma once

namespace GameServer
{
	namespace Controllers
	{
		namespace CharacterActions
		{
			enum Type
			{
				IDLE = 0,
				MOVE_FORWARD,
				MOVE_BACKWARD,
				ROTATE_LEFT,
				ROTATE_RIGHT,
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
	} // Controllers
} // GameServer