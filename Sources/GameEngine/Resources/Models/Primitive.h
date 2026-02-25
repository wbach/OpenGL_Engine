#pragma once
#include "GameEngine/Resources/Models/BoundingBox.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "Model.h"
#include "Primitives.h"
#
namespace GameEngine
{
class Primitive : public Model
{
public:
    Primitive(GraphicsApi::IGraphicsApi& api, PrimitiveType type)
        : Model(api)
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