#include "GameObject.h"
#include "Types.h"

uint32 CGameObject::s_id = 0;

CGameObject::CGameObject()
	: id(s_id++)
{
}

uint32 CGameObject::GetId() const
{
	return id;
}
