#include "GameObject.h"
#include "../Utils/Types.h"

uint CGameObject::s_ID = 0;

CGameObject::CGameObject()
	: m_ID(s_ID++)
{
}