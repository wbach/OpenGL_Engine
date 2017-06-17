#pragma once
#include "../Camera/Camera.h"
#include "../Lights/Light.h"
#include "../Time/DayNightCycle.h"
#include "../Objects/GameObject.h"
#include "../Resources/ResourceManager.h"
#include "Types.h"
#include <vector>
#include <list>
#include <memory>

//Object in scene are in grid (one grid size)
const uint32 OBJECT_GRID_SIZE = 500;
// const int grid count y in worlds
const uint32 OBJECT_GRID_COUNT = 100;

class CScene
{
public:
	CScene();

	virtual		 ~CScene() {}
	virtual int	 Initialize() = 0;
	virtual void PostInitialize() {};
	virtual int	 Update() = 0;

	//Add Entities
	CGameObject* AddGameObject(CGameObject* object, const vec3& position = vec3(0.f));

	//GetObjects
    std::list<CGameObject*> GetObjectInRange(const vec3& position, float range);
    const std::list<std::unique_ptr<CGameObject>>& GetGameObjects() { return gameObjects; }

	//Cameras
	CCamera* GetCamera();
	
	//Lights
	const CLight& GetDirectionalLight() const;
	const std::vector<CLight>&	GetLights() const;
    const CDayNightCycle& GetDayNightCycle() const { return dayNightCycle; }

	//Resources
    CResourceManager& GetResourceManager() { return resourceManager; }

    float GetDeltaTime() { return deltaTime; }
    float GetGlobalTime() { return gloabalTime; }

public:
    uint32 objectCount;

protected:
	//Minimum one light on scene only (night - decrease strength)
    CDayNightCycle		dayNightCycle;
    CLight				directionalLight;

    std::vector<CLight>	lights;

    std::list<std::unique_ptr<CGameObject>>  gameObjects;

    std::unique_ptr<CCamera> camera;

    CResourceManager resourceManager;

    float deltaTime = 0;
    float gloabalTime = 0.f;
};
