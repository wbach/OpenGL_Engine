#include "DebugDrawer.h"

#include "Converter.h"

namespace GameEngine
{
namespace Physics
{
BulletDebugDrawer::BulletDebugDrawer()
{
}
BulletDebugDrawer::~BulletDebugDrawer()
{
}
void BulletDebugDrawer::clear()
{
    lastMeshSize_ = lineMesh_.positions_.size();
    lineMesh_.colors_.clear();
    lineMesh_.positions_.clear();
    lineMesh_.positions_.reserve(lastMeshSize_);
    lineMesh_.colors_.reserve(lastMeshSize_);
}
void BulletDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    lineMesh_.positions_.push_back(from.x());
    lineMesh_.positions_.push_back(from.y());
    lineMesh_.positions_.push_back(from.z());
    lineMesh_.positions_.push_back(to.x());
    lineMesh_.positions_.push_back(to.y());
    lineMesh_.positions_.push_back(to.z());

    lineMesh_.colors_.push_back(color.x());
    lineMesh_.colors_.push_back(color.y());
    lineMesh_.colors_.push_back(color.z());
    lineMesh_.colors_.push_back(color.x());
    lineMesh_.colors_.push_back(color.y());
    lineMesh_.colors_.push_back(color.z());
    // graphicsApi_.DrawLine(Convert(color), Convert(from), Convert(to));
}
void BulletDebugDrawer::drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &)
{
}
void BulletDebugDrawer::reportErrorWarning(const char *)
{
}
void BulletDebugDrawer::draw3dText(const btVector3 &, const char *)
{
}
void BulletDebugDrawer::setDebugMode(int)
{
}
int BulletDebugDrawer::getDebugMode(void) const
{
    return 3;
}
const GraphicsApi::LineMesh &BulletDebugDrawer::getMesh() const
{
    return lineMesh_;
}
void BulletDebugDrawer::SetNeedUpdateBuffer(bool v)
{
    lineMesh_.updateBufferNeeded = v;
}
}  // namespace Physics
}  // namespace GameEngine