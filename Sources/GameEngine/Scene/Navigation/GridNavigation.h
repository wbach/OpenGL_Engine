#pragma once
#include <Logger/Log.h>

#include <set>

#include "INavigationProvider.h"
#include "NavNode.h"

namespace GameEngine
{
class ENGINE_API GridNavigation : public INavigationProvider
{
public:
    GridNavigation(const vec3&, int w, int h, float size);

    void BakeTerrain(const TerrainHeightGetter&, float maxClimbAngle) override;
    std::vector<vec3> CalculatePath(const vec3& startPos, const vec3& targetPos) override;
    void AddObstacle(const BoundingBox& box) override;
    bool IsWalkable(const vec3& position);
    void SetWalkable(int x, int y, bool walkable);
    const std::vector<NavNode>& GetNodes() const;
    int GetIndexFromWorldPos(const vec3&);
    std::vector<vec3> SmoothPath(const std::vector<vec3>& path);
    bool HasLineOfSight(const vec3& start, const vec3& end);

private:
    vec2ui WorldToGrid(const vec3&);
    std::vector<NavNode*> GetNeighbors(NavNode* node);
    std::vector<vec3> RetracePath(NavNode* startNode, NavNode* endNode);
    float GetDistance(NavNode* a, NavNode* b);
    vec3 GetWorldPosFromIndex(int x, int y, float worldHeight = 0.0f);

private:
    std::vector<NavNode> nodes;
    int width, height;
    float cellSize;
    vec3 origin;
};
}  // namespace GameEngine
