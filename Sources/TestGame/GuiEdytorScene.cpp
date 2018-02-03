#include "GuiEdytorScene.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"
#include "Thread.hpp"

GuiEdytorScene::GuiEdytorScene()
	: GameEngine::Scene("GuiEdytorScene")
	, attached(false)
{
}

GuiEdytorScene::~GuiEdytorScene()
{
}

int GuiEdytorScene::Initialize()
{
	GameEngine::Renderer::Gui::GuiTextureElement guiTexture;
	guiTexture.texture = resourceManager.GetTextureLaoder().LoadTexture("GUI/character_select.jpg", false);
	renderersManager_->GuiTexture("bg") = guiTexture;

	vec3 textColor = glm::vec3(20.f / 255.f, 20.f / 255.f, 20.f / 255.f);

	currentType_.text = "Current Type :" + GuiTypeToString(currentEditElement_.first);
	currentType_.colour = textColor;
	currentType_.position = vec2(-0.8, 0.8);

	currentElement_.text = "Element :";
	currentElement_.colour = textColor;
	currentElement_.position = vec2(-0.8, 0.7);

	renderersManager_->GuiText("Type") = currentType_;
	renderersManager_->GuiText("Element") = currentElement_;

	return 0;
}

void GuiEdytorScene::PostInitialize()
{
}

int GuiEdytorScene::Update(float deltaTime)
{
	currentEditElement_.first = GuiEditingType::Text; // texures right now are nulls

	if (attached)
	{
		auto mouseMove = inputManager_->GetMousePosition();
		Log(Utils::ToString(mouseMove));

		if (currentEditElement_.first == GuiEditingType::Texture)
		{
			std::string name = "GuiTexture" + std::to_string(currentEditElement_.second);
			renderersManager_->GuiTexture(name).SetPosition(mouseMove);
		}

		if (currentEditElement_.first == GuiEditingType::Text)
		{
			std::string name = "GuiText" + std::to_string(currentEditElement_.second);
			Log(name);

			if(!guiTexts_.empty())
				guiTexts_[currentEditElement_.second].position = mouseMove;
			renderersManager_->GuiText(name).position = mouseMove;

		}

	}

	renderersManager_->GuiText("Type").text = "Current Type :" + GuiTypeToString(currentEditElement_.first);
	renderersManager_->GuiText("Element").text = "Current Element :" + std::to_string(currentEditElement_.second);


	//if (inputManager_->GetKeyDown(KeyCodes::TAB))
	//{
	//	NextType();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}

	//if (inputManager_->GetKeyDown(KeyCodes::A))
	//{
	//	PreviousElement();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}

	//if (inputManager_->GetKeyDown(KeyCodes::D))
	//{
	//	NextElement();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}

	//if (inputManager_->GetKeyDown(KeyCodes::S))
	//{
	//	Save();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}

	//if (inputManager_->GetKeyDown(KeyCodes::ENTER))
	//{
	//	AddElement();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}

	//if (inputManager_->GetKeyDown(KeyCodes::SPACE))
	//{
	//	attached = !attached;
	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//}

	return 0;
}

std::string GuiEdytorScene::GuiTypeToString(GuiEditingType t)
{
	if (t == GuiEditingType::Texture) return "Texture";
	if (t == GuiEditingType::Text) return "Text";
	return std::string();
}

void GuiEdytorScene::NextType()
{
	auto i = static_cast<uint32>(currentEditElement_.first);
	++i;

	if (i >= 2)
		i = 0;

	currentEditElement_.first = static_cast<GuiEditingType>(i);
	currentEditElement_.second = 0;
}

void GuiEdytorScene::PreviousType()
{
	auto i = static_cast<uint32>(currentEditElement_.first);
	--i;

	if (i < 0)
		i = 1;

	currentEditElement_.first = static_cast<GuiEditingType>(i);
	currentEditElement_.second = 0;
}

void GuiEdytorScene::NextElement()
{
	++currentEditElement_.second;

	uint32 size = 0;
	if (currentEditElement_.first == GuiEditingType::Texture)
		size = guiTextures_.size();
	else if (currentEditElement_.first == GuiEditingType::Text)
		size = guiTexts_.size();

	if (currentEditElement_.second >= size)
		currentEditElement_.second = 0;
}

void GuiEdytorScene::PreviousElement()
{
	uint32 size = 0;
	if (currentEditElement_.first == GuiEditingType::Texture)
		size = guiTextures_.size();
	else if (currentEditElement_.first == GuiEditingType::Text)
		size = guiTexts_.size();

	if (currentEditElement_.second == 0)
	{
		currentEditElement_.second = size - 1;
		return;
	}

	--currentEditElement_.second;
}

void GuiEdytorScene::AddElement()
{
	if (currentEditElement_.first == GuiEditingType::Texture)
	{
		currentEditElement_.second = guiTextures_.size();
		guiTextures_.push_back({});
	}
	else if (currentEditElement_.first == GuiEditingType::Text)
	{
		currentEditElement_.second = guiTexts_.size();
		SGuiTextElement text;
		text.text = "Sample text " + std::to_string(currentEditElement_.second);
		std::string name = "GuiText" + std::to_string(currentEditElement_.second);
		renderersManager_->GuiText(name) = text;
		guiTexts_.push_back(text);
		Log(name);
	}
}

void GuiEdytorScene::Save()
{
	std::ofstream f("./guiEditorOut.txt");
	f << "Texts:\n";
	for (const auto& t : guiTexts_)
	{
		//f << "**************************\n";
		f << "\n";
		f << "ElementText_var.text = \"" << t.text << "\";\n";
		f << "ElementText_var.colour = vec3(" << t.colour.x << ", " << t.colour.y << ", " << t.colour.z << ");\n";
		f << "ElementText_var.position = vec2(" << t.position.x << ", " << t.position.y  << ");\n";
		f << "ElementText_var.m_size = \"" << t.m_size << "\";\n";
		f << "\n";
		//f << "**************************\n";
	}
	f << "Textures:\n";
	int i = 0;
	for (const auto& t : guiTextures_)
	{
		f << "**********\n";
		f << std::to_string(i++) << "\n";
		f << "Position:" << Utils::ToString(t.GetPosition()) << "\n";
		f << "\n";
	}
	f.close();
}
