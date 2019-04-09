
#include "GetCharacterState.h"
#include "Common/MessageHandling/Dispatcher.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "TestGame/MRpg/Handlers/GetCharacter/GetCharactersHandler.h"
#include "UtilsNetwork/Messages/GetCharacters/GetCharactersMsgReq.h"
#include "UtilsNetwork/Gateway.h"
#include "Logger/Log.h"

namespace MmmoRpg
{
	GetCharacterState::GetCharacterState(common::Dispacher& dispatcher, GameEngine::Renderer::RenderersManager& rendererManager, Network::CGateway& gateway, std::vector<CharacterSlot>& charactersData)
		: dispatcher_(dispatcher)
		, gateway_(gateway)
		, rendererManager_(rendererManager)
		, charactersData_(charactersData)
		, itemsTextColour_(0, 162.f / 255.f, 232.f / 255.f)
	{
		dispatcher_.AddHandler("GetCharacterStateHandler", new GetCharactersHandler(std::bind(&GetCharacterState::GetCharacters, this, std::placeholders::_1)));
		SendGetCharacter();
	}
	GetCharacterState::~GetCharacterState()
	{
		dispatcher_.RemoveHandler("GetCharacterStateHandler");
	}
	void GetCharacterState::SendGetCharacter()
	{
		Log("SelectCharacterScene::SendGetCharacter()");
		auto getCharactersMsgReq = std::make_unique<Network::GetCharactersMsgReq>();
		gateway_.Send(getCharactersMsgReq.get());
		sentTime_ = std::chrono::high_resolution_clock::now();
	}
	void GetCharacterState::Update()
	{
		auto now = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - sentTime_).count() < 5)
			return;

		Log("SelectCharacterScene::WaitForSelectCharacterResp : time out. Resend msg.");
		SendGetCharacter();
	}
	void GetCharacterState::GetCharacters(const std::vector<Network::CharacterInfo>& characters)
	{
		for (auto& character : characters)
			AddSlot(character);

		status_ = StateStatus::DONE;
	}
	void GetCharacterState::AddSlot(const Network::CharacterInfo& info)
	{
		vec2 offsetX(0.25f, 0.f);
		vec2 offsetY(0.0f, -0.1f);
		vec2 startPosition(-0.441818, -0.477143);
		float size = .5f;

		uint32 slotId = charactersData_.size();
		vec2 finalPos = startPosition + offsetX * static_cast<float>(slotId);

		CharacterSlot newSlot;
		//newSlot.texts[CharacterSlot::TextType::NICK].text = info.GetName();
		//newSlot.texts[CharacterSlot::TextType::NICK].m_size = size;
		//newSlot.texts[CharacterSlot::TextType::NICK].colour = itemsTextColour_;
		//newSlot.texts[CharacterSlot::TextType::NICK].position = finalPos + offsetY;
		//newSlot.texts[CharacterSlot::TextType::LVL].m_size = size;
		//newSlot.texts[CharacterSlot::TextType::LVL].text = "Lvl : " + std::to_string(info.lvl_);
		//newSlot.texts[CharacterSlot::TextType::LVL].colour = itemsTextColour_;
		//newSlot.texts[CharacterSlot::TextType::LVL].position = finalPos + 2.f*offsetY;
		//newSlot.texts[CharacterSlot::TextType::CLASSNAME].m_size = size;
		//newSlot.texts[CharacterSlot::TextType::CLASSNAME].text = "Class id : " + std::to_string(info.classId_);
		//newSlot.texts[CharacterSlot::TextType::CLASSNAME].colour = itemsTextColour_;
		//newSlot.texts[CharacterSlot::TextType::CLASSNAME].position = finalPos + 3.f*offsetY;;
		newSlot.characterInfo = info;
		SlotToRenderer(newSlot, slotId);
		charactersData_.push_back(newSlot);
	}
	void GetCharacterState::SlotToRenderer(const CharacterSlot& slot, uint32 id)
	{
		int i = 0;
		for (const auto& text : slot.texts)
		{
			auto name = "Slot_" + std::to_string(id) + "_" + std::to_string(i++);
			//rendererManager_.GuiText(name) = text.second;
		}
	}
}