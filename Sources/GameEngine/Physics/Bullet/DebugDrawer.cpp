#include "DebugDrawer.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Converter.h"

namespace GameEngine
{
namespace Physics
{
BulletDebugDrawer::BulletDebugDrawer(IGraphicsApi& graphicsApi)
    : graphicsApi_(graphicsApi)
{
}
void BulletDebugDrawer::SetMatrices(const mat4& viewMatrix, const mat4& projectionMatrix)
{
    graphicsApi_.UseShader(0);
    graphicsApi_.LoadViewMatrix(viewMatrix);
    graphicsApi_.LoadProjectionMatrix(projectionMatrix);
}
void BulletDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    graphicsApi_.DrawLine(Convert(color), Convert(from), Convert(to) );
}
void BulletDebugDrawer::drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int,
                                                          const btVector3 &)
{
}
void BulletDebugDrawer::reportErrorWarning(const char *)
{
}
void BulletDebugDrawer::draw3dText(const btVector3 &, const char *)
{
}
void BulletDebugDrawer::setDebugMode(int p)
{
    m = p;
}
int BulletDebugDrawer::getDebugMode(void) const
{
    return 3;
}
}  // namespace Physics
}  // namespace GameEngine