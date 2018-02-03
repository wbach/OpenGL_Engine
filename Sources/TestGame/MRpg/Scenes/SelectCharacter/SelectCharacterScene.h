#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../UtilsNetwork/Messages/GetCharacters/CharacterInfo.h"
#include "../GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "../../MrpgGameContext.h"
#include "optional.hpp"
#include <vector>

namespace Network
{
	class CGateway;
} // Network

namespace MmmoRpg
{
	struct CharacterSlot
	{
		enum TextType
		{
			LVL,
			CLASSNAME,
			NICK			
		};
		Network::CharacterInfo characterInfo;
		std::unordered_map<TextType, SGuiTextElement> texts;
	};

	class SelectCharacterScene : public GameEngine::Scene
	{
		enum State
		{
			GET_CHARACTER,
			SELECT_CHARACTER,
			WAIT_FOR_SELECT_CHARACTER_RESP,
			END
		};

	public:
		SelectCharacterScene(Network::CGateway& gateway, MrpgGameContext& gameContext);
		virtual ~SelectCharacterScene() override;
		virtual int		Initialize() override;
		virtual void	PostInitialize() override;
		virtual int		Update(float deltaTime) override;

	private:
		void SendSelectCharacterReq();
		void WaitForSelectCharacterResp();
		void SendGetCharacter();
		void WaitForGetCharacterResp();
		void SlotToRenderer(const CharacterSlot& slot, uint32 id);
		void AddSlot(const Network::CharacterInfo& info);
		void SetCurrentChoiceText();
		void Pause();
		void CheckRangeSelectedCharacter();
		void ChangeSelectedCharacter(int8 dir);
		void SelectCharacterState();

	private:
		State state_;
		wb::optional<int8> currentSelectCharacterId_;
		SGuiTextElement characterSelectText_;
		Network::CGateway& gateway_;
		std::vector<CharacterSlot> charactersData_;
		vec3 itemsTextColour_;
		Timepoint selectedCharacterMsgSentTime_;
		MrpgGameContext& gameContext_;
	};
}