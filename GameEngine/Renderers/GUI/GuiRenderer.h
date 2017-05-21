#pragma once
#include <memory>
#include <vector>
#include <GL/glew.h>
#include "Text/GuiText.h"
#include "../Renderer.h"

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
	// create unique_ptr from ptr
	void AddElement(CGuiElement* element);

private: 
    std::vector<std::unique_ptr<CGuiElement>> guiElements;
};
