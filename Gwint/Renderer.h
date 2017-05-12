#pragma once
#include "Cards/Card.h"
#include "Player.h"
#include <Renderers/SimpleRenderer.h>

class CGwitRenderer : public SimpleRenderer
{
public:
	CGwitRenderer(SProjection* projection_matrix)
		: SimpleRenderer(projection_matrix)
		, renderer_quad(nullptr)
		, bg_texture(nullptr)
	{}


	virtual void Init() override
	{
		SimpleRenderer::Init();
	}
	virtual void Render(CScene* scene) override
	{
		if (renderer_quad == nullptr)
			return;
		
		cards.reverse();
		for (const auto& t : cards)
		{
			SMaterial material;
			material.m_DiffuseTexture = t.texture;
			renderer_quad->GetModel()->SetMaterial(material);
			renderer_quad->m_WorldTransform.SetScale(t.scale);
			renderer_quad->m_WorldTransform.SetPosition(t.position);
			RenderModel(renderer_quad->GetModel(), renderer_quad->m_WorldTransform.GetMatrix());
		}

		if (bg_texture == nullptr)
			return;

		SMaterial material;
		material.m_DiffuseTexture = bg_texture;
		renderer_quad->GetModel()->SetMaterial(material);
		renderer_quad->m_WorldTransform.SetScale(glm::vec3(-1, -1, 1));
		renderer_quad->m_WorldTransform.SetPosition(glm::vec3(0, -0.05, -2));
		RenderModel(renderer_quad->GetModel(), renderer_quad->m_WorldTransform.GetMatrix());
	}
	CEntity* renderer_quad;
	CTexture* bg_texture;
	std::list<SCard> cards;
};