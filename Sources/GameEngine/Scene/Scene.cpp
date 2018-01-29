#include "Scene.hpp"
#include "Logger/Log.h"

CScene::CScene(const std::string & name)
	: name(name)
	, directionalLight(vec3(10000, 15000, 10000), vec3(0.8))
	, camera(new CCamera)
{}

CScene::~CScene()
{
	Log(__FUNCTION__);
}

CGameObject* CScene::AddGameObject(CGameObject* object, const vec3& position)
{
	object->worldTransform.SetPosition(position);
	gameObjects.emplace_back(object);
	return gameObjects.back().get();
}

void CScene::SetAddSceneEventCallback(AddEvent func)
{
	addSceneEvent = func;
}

std::list<CGameObject*> CScene::GetObjectInRange(const vec3 & position, float range)
{
    //int x = static_cast<uint>(position.x / OBJECT_GRID_SIZE);
    //int y = static_cast<uint>(position.z / OBJECT_GRID_SIZE);

	return std::list<CGameObject*>();// m_ObjectInGrid[x + y*OBJECT_GRID_COUNT];
}

CCamera * CScene::GetCamera()
{
	return camera.get();
}

const CLight & CScene::GetDirectionalLight() const
{
	return directionalLight;
}

const std::vector<CLight>& CScene::GetLights() const
{
	return lights;
}
