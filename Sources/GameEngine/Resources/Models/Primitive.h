#pragma once
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "Model.h"
#include "Primitives.h"
#
namespace GameEngine
{
class Primitive : public Model
{
public:
    Primitive(PrimitiveType type, BoundingBox&& box)
        : Model(box)
        , type_(type)
    {
    }

    PrimitiveType GetType() const
    {
        return type_;
    }

private:
    PrimitiveType type_;
};
}  // namespace GameEngine