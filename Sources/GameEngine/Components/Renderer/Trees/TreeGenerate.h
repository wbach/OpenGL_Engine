#pragma once
#include "GraphicsApi/MeshRawData.h"

namespace GameEngine
{
GraphicsApi::MeshRawData generateTree(int attractorCount = 9000, float crownRadius = 10.0f, float crownHeight = 12.0f,
                                      float attractionDistance = 2.0f, float killDistance = 1.2f, float segmentLength = 0.8f,
                                      float trunkOffsetY = 3.f);
}
