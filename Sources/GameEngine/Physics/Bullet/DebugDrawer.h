#pragma once
#include <LinearMath/btIDebugDraw.h>
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

namespace GameEngine
{
namespace Physics
{
class BulletDebugDrawer : public btIDebugDraw
{
public:
    BulletDebugDrawer();
    ~BulletDebugDrawer();
    void clear();
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
    void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) override;
    void reportErrorWarning(const char *) override;
    void draw3dText(const btVector3 &, const char *) override;
    void setDebugMode(int p) override;
    int getDebugMode(void) const override;
    const GraphicsApi::LineMesh& getMesh() const;
    void SetNeedUpdateBuffer(bool);

private:
    GraphicsApi::LineMesh lineMesh_;
    size_t lastMeshSize_ = 0;
};
}  // namespace Physics
}  // namespace GameEngine
