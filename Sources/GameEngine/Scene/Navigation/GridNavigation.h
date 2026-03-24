#pragma once
#include <set>

#include "INavigationProvider.h"
#include "Logger/Log.h"
#include "NavNode.h"

namespace GameEngine
{
class GridNavigation : public INavigationProvider
{
public:
    GridNavigation(int w, int h, float size);

    std::vector<vec3> CalculatePath(const vec3& startPos, const vec3& targetPos) override;
    bool IsWalkable(const vec3& position) override;
    void SetWalkable(int x, int y, bool walkable);
    const std::vector<NavNode>& GetNodes() const;

private:
    vec2ui WorldToGrid(vec3 pos);
    int GetIndexFromWorldPos(glm::vec3 worldPos);
    std::vector<NavNode*> GetNeighbors(NavNode* node);
    std::vector<vec3> RetracePath(NavNode* startNode, NavNode* endNode);
    float GetDistance(NavNode* a, NavNode* b);

private:
    std::vector<NavNode> nodes;
    int width, height;
    float cellSize;
};
}  // namespace GameEngine
