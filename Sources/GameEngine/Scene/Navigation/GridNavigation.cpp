#include "GridNavigation.h"

namespace GameEngine
{

GridNavigation::GridNavigation(int w, int h, float size)
    : width(w)
    , height(h)
    , cellSize(size)
{
    nodes.resize(width * height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx               = y * width + x;
            nodes[idx].x          = x;
            nodes[idx].y          = y;
            nodes[idx].isWalkable = true;
        }
    }
}
std::vector<vec3> GridNavigation::CalculatePath(const vec3& startPos, const vec3& targetPos)
{
    for (auto& node : nodes)
    {
        node.gCost  = std::numeric_limits<float>::max();
        node.hCost  = 0;
        node.parent = nullptr;
    }

    int startIdx  = GetIndexFromWorldPos(startPos);
    int targetIdx = GetIndexFromWorldPos(targetPos);

    if (startIdx == -1 || targetIdx == -1)
    {
        LOG_WARN << "Get index failure";
        return {};
    }

    auto* startNode  = &nodes[startIdx];
    auto* targetNode = &nodes[targetIdx];

    LOG_DEBUG << "DEBUG: StartIdx: " << startIdx << " TargetIdx: " << targetIdx;

    startNode->gCost = 0;
    startNode->hCost = GetDistance(startNode, targetNode);

    std::vector<NavNode*> openList;
    std::set<NavNode*> closedList;

    openList.push_back(startNode);

    while (not openList.empty())
    {
        auto currentIt =
            std::min_element(openList.begin(), openList.end(), [](NavNode* a, NavNode* b) { return a->fCost() < b->fCost(); });

        NavNode* currentNode = *currentIt;

        if (currentNode == targetNode)
        {
            LOG_DEBUG << "RetracePath";
            return RetracePath(startNode, targetNode);
        }

        openList.erase(currentIt);
        closedList.insert(currentNode);

        for (auto* neighbor : GetNeighbors(currentNode))
        {
            if (not neighbor->isWalkable or closedList.count(neighbor))
                continue;

            auto newMovementCostToNeighbor = currentNode->gCost + GetDistance(currentNode, neighbor);

            auto inOpen = std::find(openList.begin(), openList.end(), neighbor) != openList.end();

            if (newMovementCostToNeighbor < neighbor->gCost or not inOpen)
            {
                neighbor->gCost  = newMovementCostToNeighbor;
                neighbor->hCost  = GetDistance(neighbor, targetNode);
                neighbor->parent = currentNode;

                if (not inOpen)
                    openList.push_back(neighbor);
            }
        }
    }

    LOG_DEBUG << "return empty";
    return {};
}
bool GridNavigation::IsWalkable(const vec3& position)
{
    return true;
}
void GridNavigation::SetWalkable(int x, int y, bool walkable)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        nodes[y * width + x].isWalkable = walkable;
    }
}
const std::vector<NavNode>& GridNavigation::GetNodes() const
{
    return nodes;
}
vec2ui GridNavigation::WorldToGrid(vec3 pos)
{
    return {(int)(pos.x / cellSize), (int)(pos.z / cellSize)};
}
int GridNavigation::GetIndexFromWorldPos(glm::vec3 worldPos)
{
    int x = static_cast<int>(worldPos.x / cellSize);
    int z = static_cast<int>(worldPos.z / cellSize);

    if (x < 0 || x >= width || z < 0 || z >= height)
        return -1;

    return z * width + x;
}
std::vector<NavNode*> GridNavigation::GetNeighbors(NavNode* node)
{
    std::vector<NavNode*> neighbors;

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x == 0 && y == 0)
                continue;

            int checkX = node->x + x;
            int checkY = node->y + y;

            if (checkX >= 0 && checkX < width && checkY >= 0 && checkY < height)
            {
                if (std::abs(x) == 1 && std::abs(y) == 1)
                {
                    if (not nodes[node->y * width + checkX].isWalkable or not nodes[checkY * width + node->x].isWalkable)
                    {
                        continue;
                    }
                }

                neighbors.push_back(&nodes[checkY * width + checkX]);
            }
        }
    }
    return neighbors;
}
std::vector<vec3> GridNavigation::RetracePath(NavNode* startNode, NavNode* endNode)
{
    std::vector<vec3> path;
    NavNode* currentNode = endNode;

    while (currentNode != startNode)
    {
        path.push_back(
            vec3((currentNode->x * cellSize) + (cellSize * 0.5f), 0.0f, (currentNode->y * cellSize) + (cellSize * 0.5f)));

        currentNode = currentNode->parent;
    }

    std::reverse(path.begin(), path.end());
    return path;
}
float GridNavigation::GetDistance(NavNode* a, NavNode* b)
{
    int dstX = std::abs(a->x - b->x);
    int dstY = std::abs(a->y - b->y);

    if (dstX > dstY)
        return 1.41f * dstY + 1.0f * (dstX - dstY);

    return 1.41f * dstX + 1.0f * (dstY - dstX);
}
}  // namespace GameEngine
