#include "Scene.hpp"

CScene::CScene()
	: m_DirectionalLight(glm::vec3(10000, 15000, 10000), glm::vec3(0.8))
{
}

CGameObject* CScene::AddGameObject(CGameObject* object, const glm::vec3& position)
{
	object->m_WorldTransform.SetPosition(position);
	m_GameObjects.emplace_back(object);
	return m_GameObjects.back().get();
}

std::list<CGameObject*> CScene::GetObjectInRange(const glm::vec3 & position, float range)
{
	int x = static_cast<uint>(position.x / OBJECT_GRID_SIZE);
	int y = static_cast<uint>(position.z / OBJECT_GRID_SIZE);

	return std::list<CGameObject*>();// m_ObjectInGrid[x + y*OBJECT_GRID_COUNT];
}

CCamera * CScene::GetCamera()
{
	return m_Camera.get();
}

const CLight & CScene::GetDirectionalLight() const
{
	return m_DirectionalLight;
}

const std::vector<CLight>& CScene::GetLights() const
{
	return m_Lights;
}
