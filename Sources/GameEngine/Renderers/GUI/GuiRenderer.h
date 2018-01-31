#pragma once
#include "GuiElement.h"
#include "../Renderer.h"
#include <memory>
#include <vector>
#include <GL/glew.h>

class CGUIRenderer : public CRenderer
{
public:
	CGUIRenderer() {}
	virtual ~CGUIRenderer() {}
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) {}
	virtual void UnSubscribeAll() override;
	// create unique_ptr from ptr
	void AddElement(CGuiElement* element);

private: 
    std::vector<CGuiElementPtr> guiElements;
};
