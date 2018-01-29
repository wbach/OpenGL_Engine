#pragma once
#include "SceneEvents.h"
#include "../Camera/Camera.h"
#include "../Lights/Light.h"
#include "../Objects/GameObject.h"
#include "../Resources/ResourceManager.h"
#include "../Time/DayNightCycle.h"
#include "Types.h"
#include <list>
#include <memory>
#include <vector>

// Object in scene are in grid (one grid size)
const uint32 OBJECT_GRID_SIZE = 500;
// const int grid count y in worlds
const uint32 OBJECT_GRID_COUNT = 100;

class CScene
{
public:
	CScene(const std::string& name);

	virtual ~CScene();
    virtual int Initialize() = 0;
    virtual void PostInitialize(){}
    virtual int Update(float deltaTime) = 0;
	const std::string& GetName() { return name; }
    // Add Entities
    CGameObject* AddGameObject(CGameObject* object, const vec3& position = vec3(0.f));
	void SetAddSceneEventCallback(GameEngine::AddEvent func);

    // GetObjects
    std::list<CGameObject*> GetObjectInRange(const vec3& position, float range);
	inline const std::list<std::unique_ptr<CGameObject>>& GetGameObjects();

    // Cameras
    CCamera* GetCamera();

    // Lights
    const CLight& GetDirectionalLight() const;
    const std::vector<CLight>& GetLights() const;
	inline const CDayNightCycle& GetDayNightCycle() const;

    // Resources
	inline CResourceManager& GetResourceManager();
	inline float GetGlobalTime();

public:
    uint32 objectCount;

protected:
	std::string name;
	GameEngine::AddEvent addSceneEvent;
    // Minimum one light on scene only (night - decrease strength)
    CDayNightCycle dayNightCycle;
    CLight directionalLight;

    std::vector<CLight> lights;

    std::list<std::unique_ptr<CGameObject>> gameObjects;

    std::unique_ptr<CCamera> camera;

    CResourceManager resourceManager;

    float gloabalTime = 0.f;
};

inline const std::list<std::unique_ptr<CGameObject>>& CScene::GetGameObjects()
{
	return gameObjects;
}

inline const CDayNightCycle& CScene::GetDayNightCycle() const
{
	return dayNightCycle;
}

// Resources
inline CResourceManager& CScene::GetResourceManager()
{
	return resourceManager;
}

inline float CScene::GetGlobalTime()
{
	return gloabalTime;
}