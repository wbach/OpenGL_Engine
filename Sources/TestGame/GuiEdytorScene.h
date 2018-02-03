#pragma once
#include "../GameEngine/Scene/Scene.hpp"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "../GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
#include <unordered_map>

class GuiEdytorScene : public GameEngine::Scene
{
public:
	enum class GuiEditingType
	{
		Texture = 0,
		Text
	};
	GuiEdytorScene();
	virtual ~GuiEdytorScene() override;
	virtual int	 Initialize() override;
	virtual void PostInitialize() override;
	virtual int  Update(float deltaTime) override;

private:
	std::string GuiTypeToString(GuiEditingType);
	void NextType();
	void PreviousType();
	void NextElement();
	void PreviousElement();
	void AddElement();
	void Save();

private:
	SGuiTextElement currentType_;
	SGuiTextElement currentElement_;

	std::pair<GuiEditingType, uint32> currentEditElement_;

	std::vector<SGuiTextElement> guiTexts_;
	std::vector<GameEngine::Renderer::Gui::GuiTextureElement> guiTextures_;

	bool attached;
};
