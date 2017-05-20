#pragma once
#include "../Camera/Camera.h"
#include "../Lights/Light.h"
#include "../Time/DayNightCycle.h"
#include "../Objects/GameObject.h"
#include "../Resources/ResourceManager.h"
#include "../Utils/Types.h"
#include <vector>
#include <list>
#include <memory>

//Object in scene are in grid (one grid size)
const uint OBJECT_GRID_SIZE = 500;
// const int grid count y in worlds
const uint OBJECT_GRID_COUNT = 100;

class CScene
{
public:
	CScene();

	virtual		 ~CScene() {}
	virtual int	 Initialize() = 0;
	virtual void PostInitialize() {};
	virtual int	 Update() = 0;

	//Add Entities
	CGameObject* AddGameObject(CGameObject* object, const glm::vec3& position = glm::vec3(0.f));

	//GetObjects
    std::list<CGameObject*> GetObjectInRange(const glm::vec3& position, float range);
	const std::list<std::unique_ptr<CGameObject>>& GetGameObjects() { return m_GameObjects; }

	//Cameras
	CCamera* GetCamera();
	
	//Lights
	const CLight& GetDirectionalLight() const;
	const std::vector<CLight>&	GetLights() const;
	const CDayNightCycle& GetDayNightCycle() const { return m_DayNightCycle; }

	//Resources
	CResourceManager& GetResourceManager() { return m_ResourceManager; }

	float GetDeltaTime() { return m_DeltaTime; }
	float GetGlobalTime() { return m_GloabalTime; }

	uint m_ObjectCount;
protected:
	//Minimum one light on scene only (night - decrease strength)
	CDayNightCycle		m_DayNightCycle;
	CLight				m_DirectionalLight;

	std::vector<CLight>	m_Lights;

    std::list<std::unique_ptr<CGameObject>>  m_GameObjects;

    std::unique_ptr<CCamera> m_Camera;

	CResourceManager m_ResourceManager;

	float m_DeltaTime = 0;
	float m_GloabalTime = 0.f;	
};
