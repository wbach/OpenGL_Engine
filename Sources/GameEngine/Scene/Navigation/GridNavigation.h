#pragma once
#include <Logger/Log.h>

#include <set>

#include "INavigationProvider.h"
#include "NavNode.h"

namespace GameEngine
{
class BoundingBox;
class TerrainHeightGetter;
class GridNavigation : public INavigationProvider
{
public:
    GridNavigation(const vec3&, int w, int h, float size);

    void BakeTerrain(const TerrainHeightGetter&, float maxClimbAngle);
    std::vector<vec3> CalculatePath(const vec3& startPos, const vec3& targetPos) override;
    bool IsWalkable(const vec3& position) override;
    void SetWalkable(int x, int y, bool walkable);
    const std::vector<NavNode>& GetNodes() const;
    void AddObstacle(const BoundingBox& box);
    int GetIndexFromWorldPos(const vec3&);

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
