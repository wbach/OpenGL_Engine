#pragma once
#include "GuiElement.h"
#include "../Renderer.h"
#include <memory>
#include <vector>

class CGUIRenderer : public CRenderer
{
public:
	CGUIRenderer() {}
	virtual ~CGUIRenderer() {}
	virtual void Init() override;
	virtual void PrepareFrame(GameEngine::Scene* scene) override;
	virtual void Render(GameEngine::Scene* scene) override;
	virtual void EndFrame(GameEngine::Scene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) {}
	virtual void UnSubscribeAll() override;
	// create unique_ptr from ptr
	void AddElement(CGuiElement* element);

private: 
    std::vector<CGuiElementPtr> guiElements;
};
