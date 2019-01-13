#pragma once
#include <LinearMath/btIDebugDraw.h>
#include "Types.h"

namespace GameEngine
{
class IGraphicsApi;

namespace Physics
{
class BulletDebugDrawer : public btIDebugDraw
{
public:
    BulletDebugDrawer(IGraphicsApi& graphicsApi);
    void SetMatrices(const mat4 &viewMatrix, const mat4 &projectionMatrix);
    virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
    virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &);
    virtual void reportErrorWarning(const char *);
    virtual void draw3dText(const btVector3 &, const char *);
    virtual void setDebugMode(int p);
    int getDebugMode(void) const;
    int m;

private:
    IGraphicsApi& graphicsApi_;
};
}  // namespace Physics
}  // namespace GameEngine
