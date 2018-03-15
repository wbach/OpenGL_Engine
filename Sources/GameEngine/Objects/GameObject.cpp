#include "GameObject.h"
#include "Types.h"
#include "GameEngine/Components/ComponentFactory.h"

uint32 CGameObject::s_id = 0;

CGameObject::CGameObject()
	: id(s_id++)
	, s_componentID(0)
{
}

uint32 CGameObject::GetId() const
{
	return id;
}

void CGameObject::AddComponent(GameEngine::Components::AbstractComponentPtr component)
{
	component->SetObjectPtr(this);
	components_[s_componentID++] = std::move(component);
}
