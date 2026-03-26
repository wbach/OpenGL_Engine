#include "GridNavigation.h"

#include <mutex>

#include "GameEngine/Components/Physics/Terrain/TerrainHeightGetter.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GameEngine/Resources/Models/Model.h"
#include "Logger/Log.h"

namespace GameEngine
{

GridNavigation::GridNavigation(const vec3& orgin, int w, int h, float size)
    : width(w)
    , height(h)
    , cellSize(size)
    , origin(orgin)
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

    LOG_DEBUG << "\n--- GRID DEBUG (Size: " << width << "x" << height << ", Cell: " << cellSize << ", Origin: " << origin.x << ","
              << origin.z << ") ---";
}
std::vector<vec3> GridNavigation::CalculatePath(const vec3& startPos, const vec3& targetPos)
{
    std::lock_guard lk(nodesMutex);
    for (auto& node : nodes)
    {
        node.gCost  = std::numeric_limits<float>::max();
        node.hCost  = 0;
        node.parent = nullptr;
    }

    int startIdx  = GetIndexFromWorldPos(startPos);
    int targetIdx = GetIndexFromWorldPos(targetPos);

    if (startIdx == -1 or targetIdx == -1)
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
    auto index = GetIndexFromWorldPos(position);

    if (index < 0 or index >= static_cast<int>(nodes.size()))
    {
        return false;
    }

    return nodes[index].isWalkable;
}
void GridNavigation::SetWalkable(int x, int y, bool walkable)
{
    if (x >= 0 and x < width and y >= 0 and y < height)
    {
        nodes[y * width + x].isWalkable = walkable;
    }
}
const std::vector<NavNode>& GridNavigation::GetNodes() const
{
    std::lock_guard lk(nodesMutex);
    return nodes;
}
vec2ui GridNavigation::WorldToGrid(const vec3& pos)
{
    return {(int)(pos.x / cellSize), (int)(pos.z / cellSize)};
}
int GridNavigation::GetIndexFromWorldPos(const vec3& worldPos)
{
    float localX = worldPos.x - origin.x;
    float localZ = worldPos.z - origin.z;

    int x = static_cast<int>(std::floor(localX / cellSize));
    int z = static_cast<int>(std::floor(localZ / cellSize));

    if (x < 0 or x >= width or z < 0 or z >= height)
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
            if (x == 0 and y == 0)
                continue;

            int checkX = node->x + x;
            int checkY = node->y + y;

            if (checkX >= 0 and checkX < width and checkY >= 0 and checkY < height)
            {
                if (std::abs(x) == 1 and std::abs(y) == 1)
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
        path.push_back(vec3(origin.x + (currentNode->x * cellSize) + (cellSize * 0.5f), currentNode->height,
                            origin.z + (currentNode->y * cellSize) + (cellSize * 0.5f)));

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
void GridNavigation::AddObstacle(const BoundingBox& box)
{
    std::lock_guard lk(nodesMutex);
    auto getX = [&](auto worldX) { return static_cast<int>(std::floor((worldX - origin.x) / cellSize)); };
    auto getZ = [&](auto worldZ) { return static_cast<int>(std::floor((worldZ - origin.z) / cellSize)); };

    auto startX = std::max(0, getX(box.min().x));
    auto endX   = std::min(width - 1, getX(box.max().x));
    auto startZ = std::max(0, getZ(box.min().z));
    auto endZ   = std::min(height - 1, getZ(box.max().z));

    for (int x = startX; x <= endX; ++x)
    {
        for (int z = startZ; z <= endZ; ++z)
        {
            SetWalkable(x, z, false);
        }
    }
}
void GridNavigation::BakeTerrain(const TerrainHeightGetter& heightGetter, float maxClimbAngle)
{
    std::lock_guard lk(nodesMutex);
    auto maxSlope = std::tan(glm::radians(maxClimbAngle)) * cellSize;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto idx      = y * width + x;
            auto worldPos = GetWorldPosFromIndex(x, y);

            auto h = heightGetter.GetHeightofTerrain(worldPos.x, worldPos.z);
            if (h.has_value())
            {
                nodes[idx].height     = h.value();
                nodes[idx].isWalkable = true;
            }
            else
            {
                nodes[idx].isWalkable = false;
                continue;
            }

            if (x > 0)
            {
                auto hPrevX = nodes[idx - 1].height;
                if (std::abs(nodes[idx].height - hPrevX) > maxSlope)
                {
                    nodes[idx].isWalkable = false;
                }
            }

            if (y > 0)
            {
                auto hPrevY = nodes[idx - width].height;
                if (std::abs(nodes[idx].height - hPrevY) > maxSlope)
                {
                    nodes[idx].isWalkable = false;
                }
            }
        }
    }
}
vec3 GridNavigation::GetWorldPosFromIndex(int x, int y, float worldHeight)
{
    return vec3(origin.x + (x * cellSize) + (cellSize * 0.5f), worldHeight, origin.z + (y * cellSize) + (cellSize * 0.5f));
}
std::vector<vec3> GridNavigation::SmoothPath(const std::vector<vec3>& path)
{
    if (path.size() < 3)
        return path;

    std::vector<vec3> smoothedPath;
    smoothedPath.push_back(path[0]);

    size_t currentIndex = 0;
    while (currentIndex < path.size() - 1)
    {
        size_t nextVisibleIndex = currentIndex + 1;

        for (size_t i = currentIndex + 2; i < path.size(); ++i)
        {
            if (HasLineOfSight(path[currentIndex], path[i]))
            {
                nextVisibleIndex = i;
            }
            else
            {
                break;
            }
        }

        smoothedPath.push_back(path[nextVisibleIndex]);
        currentIndex = nextVisibleIndex;
    }

    return smoothedPath;
}
bool GridNavigation::HasLineOfSight(const vec3& start, const vec3& end)
{
    float dist     = glm::distance(vec2(start.x, start.z), vec2(end.x, end.z));
    float stepDist = cellSize * 0.3f;
    int steps      = static_cast<int>(dist / stepDist);

    vec3 direction = glm::normalize(end - start);

    for (int i = 1; i < steps; ++i)
    {
        vec3 checkPos = start + direction * (static_cast<float>(i) * stepDist);
        int idx       = GetIndexFromWorldPos(checkPos);

        if (idx == -1 or not nodes[idx].isWalkable)
            return false;

        float t              = static_cast<float>(i) / steps;
        float expectedHeight = glm::mix(start.y, end.y, t);

        float actualHeight = nodes[idx].height;
        if (std::abs(actualHeight - expectedHeight) > (cellSize * 0.5f))
        {
            return false;
        }
    }

    return true;
}
void GridNavigation::AddObstacle(Model& model, const glm::mat4& modelTransform, float agentRadius)
{
    std::lock_guard lk(nodesMutex);
    auto worldAABB = model.transformBoundingBox(modelTransform).expanded(agentRadius);

    auto getX = [&](auto worldX) { return static_cast<int>(std::round((worldX - origin.x) / cellSize)); };
    auto getZ = [&](auto worldZ) { return static_cast<int>(std::round((worldZ - origin.z) / cellSize)); };

    auto startX = std::max(0, getX(worldAABB.min().x));
    auto endX   = std::min(width - 1, getX(worldAABB.max().x));
    auto startZ = std::max(0, getZ(worldAABB.min().z));
    auto endZ   = std::min(height - 1, getZ(worldAABB.max().z));

    struct MeshCollisionData
    {
        mat4 invTransform;
        BoundingBox localBox;
    };
    std::vector<MeshCollisionData> meshData;

    for (const auto& mesh : model.GetMeshes())
    {
        auto fullMeshTransform = modelTransform * mesh.GetMeshTransform();
        meshData.push_back({glm::inverse(fullMeshTransform), mesh.getBoundingBox().expanded(agentRadius)});
    }

    for (int x = startX; x <= endX; ++x)
    {
        for (int z = startZ; z <= endZ; ++z)
        {
            auto cellWorldPos = GetWorldPosFromIndex(x, z);

            float r = cellSize * 0.5f;
            BoundingBox cellAABB(cellWorldPos - vec3(r), cellWorldPos + vec3(r));

            bool hit = false;
            for (const auto& data : meshData)
            {
                auto localCellBox = cellAABB.transformed(data.invTransform);
                if (data.localBox.intersects(localCellBox))
                {
                    hit = true;
                    break;
                }
            }

            if (hit)
            {
                SetWalkable(x, z, false);
            }
        }
    }
}
void GridNavigation::AddPhysicsObstacle(Physics::IPhysicsApi& api, const BoundingBox& worldBB, float agentHeight)
{
    std::lock_guard lk(nodesMutex);
    LOG_DEBUG << worldBB;
    auto getX = [&](auto worldX) { return static_cast<int>(std::floor((worldX - origin.x) / cellSize)); };
    auto getZ = [&](auto worldZ) { return static_cast<int>(std::floor((worldZ - origin.z) / cellSize)); };

    float xMin = std::min(worldBB.min().x, worldBB.max().x);
    float xMax = std::max(worldBB.min().x, worldBB.max().x);
    float zMin = std::min(worldBB.min().z, worldBB.max().z);
    float zMax = std::max(worldBB.min().z, worldBB.max().z);

    // 2. Oblicz indeksy
    int sX = getX(xMin);
    int eX = getX(xMax);
    int sZ = getZ(zMin);
    int eZ = getZ(zMax);

    // 3. Dodatkowe zabezpieczenie pętli
    int startX = std::clamp(std::min(sX, eX), 0, width - 1);
    int endX   = std::clamp(std::max(sX, eX), 0, width - 1);
    int startZ = std::clamp(std::min(sZ, eZ), 0, height - 1);
    int endZ   = std::clamp(std::max(sZ, eZ), 0, height - 1);

    vec3 halfExtents(cellSize * 0.5f, cellSize * 0.5f, cellSize * 0.5f);

    for (int z = startZ; z <= endZ; ++z)
    {
        for (int x = startX; x <= endX; ++x)
        {
            int idx = z * width + x;
            if (!nodes[idx].isWalkable)
                continue;

            auto cellPos = GetWorldPosFromIndex(x, z, nodes[idx].height);

            vec3 testCenter(cellPos.x, cellPos.y, cellPos.z);

            if (api.checkBoxOverlap(testCenter, halfExtents))
            {
                nodes[idx].isWalkable = false;
            }
        }
    }
}
int GridNavigation::GetWidth() const
{
    return width;
}
int GridNavigation::GetHeight() const
{
    return height;
}
float GridNavigation::GetCellSize() const
{
    return cellSize;
}
const vec3& GridNavigation::GetOrigin() const
{
    return origin;
}
}  // namespace GameEngine
