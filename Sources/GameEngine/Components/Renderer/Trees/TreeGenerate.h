#pragma once
#include "GraphicsApi/MeshRawData.h"

namespace GameEngine
{
GraphicsApi::MeshRawData generateTree(int attractorCount = 700, float crownRadius = 3.0f, float crownHeight = 5.0f,
                                      float attractionDistance = 2.0f, float killDistance = 0.8f, float segmentLength = 0.3f, float trunkOffsetY = 1.f);
}
