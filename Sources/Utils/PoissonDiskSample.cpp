#include "PoissonDiskSample.h"

namespace Utils
{
namespace
{
struct PoissonContext
{
    vec2 min;
    vec2 max;
    float radius;
    float cellSize;

    int gridW;
    int gridH;

    std::vector<int> grid;
    std::vector<vec2> points;
    std::vector<vec2> active;

    PoissonContext(vec2 min_, vec2 max_, float r)
        : min(min_)
        , max(max_)
        , radius(r)
    {
        cellSize = radius / sqrt(2.f);

        gridW = static_cast<int>((max.x - min.x) / cellSize) + 1;
        gridH = static_cast<int>((max.y - min.y) / cellSize) + 1;

        grid.assign(gridW * gridH, -1);
    }
};
int GetRandomActiveIndex(const PoissonContext& ctx)
{
    int i = static_cast<int>(getRandomFloat(0.f, (float)ctx.active.size()));
    return glm::clamp(i, 0, (int)ctx.active.size() - 1);
}
vec2 GenerateRandomPointInAnnulus(const vec2& center, float radius)
{
    float angle = getRandomFloat(0.f, 2.f * glm::pi<float>());
    float r     = getRandomFloat(radius, 2.f * radius);

    return center + vec2(cos(angle), sin(angle)) * r;
}
bool IsValidPoint(const PoissonContext& ctx, const vec2& p)
{
    if (p.x < ctx.min.x || p.x >= ctx.max.x || p.y < ctx.min.y || p.y >= ctx.max.y)
        return false;

    int gx = (p.x - ctx.min.x) / ctx.cellSize;
    int gy = (p.y - ctx.min.y) / ctx.cellSize;

    for (int y = -2; y <= 2; y++)
        for (int x = -2; x <= 2; x++)
        {
            int nx = gx + x;
            int ny = gy + y;

            if (nx < 0 || ny < 0 || nx >= ctx.gridW || ny >= ctx.gridH)
                continue;

            int idx = ctx.grid[ny * ctx.gridW + nx];
            if (idx != -1 && glm::distance(ctx.points[idx], p) < ctx.radius)
                return false;
        }
    return true;
}
int GridIndex(const PoissonContext& ctx, const vec2& p)
{
    int gx = (p.x - ctx.min.x) / ctx.cellSize;
    int gy = (p.y - ctx.min.y) / ctx.cellSize;
    return gy * ctx.gridW + gx;
}
void AddPoint(PoissonContext& ctx, const vec2& p)
{
    ctx.points.push_back(p);
    ctx.active.push_back(p);
    ctx.grid[GridIndex(ctx, p)] = (int)ctx.points.size() - 1;
}
void RemoveActivePoint(PoissonContext& ctx, int index)
{
    ctx.active[index] = ctx.active.back();
    ctx.active.pop_back();
}
bool TryGeneratePointAround(PoissonContext& ctx, const vec2& center, int k)
{
    for (int j = 0; j < k; j++)
    {
        vec2 candidate = GenerateRandomPointInAnnulus(center, ctx.radius);

        if (IsValidPoint(ctx, candidate))
        {
            AddPoint(ctx, candidate);
            return true;
        }
    }
    return false;
}
void AddFirstPoint(PoissonContext& ctx)
{
    vec2 p{getRandomFloat(ctx.min.x, ctx.max.x), getRandomFloat(ctx.min.y, ctx.max.y)};

    ctx.points.push_back(p);
    ctx.active.push_back(p);
    ctx.grid[GridIndex(ctx, p)] = 0;
}
void ProcessActivePoint(PoissonContext& ctx, int k)
{
    int i     = GetRandomActiveIndex(ctx);
    vec2 base = ctx.active[i];

    if (!TryGeneratePointAround(ctx, base, k))
        RemoveActivePoint(ctx, i);
}
}  // namespace

std::vector<vec2> PoissonDiskSample(const vec2& min, const vec2& max, float radius, int k)
{
    PoissonContext ctx(min, max, radius);

    AddFirstPoint(ctx);

    while (!ctx.active.empty())
        ProcessActivePoint(ctx, k);

    return ctx.points;
}
}  // namespace Utils
