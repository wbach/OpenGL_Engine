#include "GraphicsObject.h"

namespace GraphicsApi
{
uint64 GraphicsObject::s_id = 0;

GraphicsObject::GraphicsObject()
    : id(s_id++)
{
}
uint64 GraphicsObject::GetId() const
{
    return id;
}
}  // namespace GraphicsApi
