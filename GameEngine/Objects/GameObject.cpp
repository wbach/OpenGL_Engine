#include "GameObject.h"
#include "../Utils/Types.h"

uint CGameObject::s_id = 0;

CGameObject::CGameObject()
	: id(s_id++)
{
}