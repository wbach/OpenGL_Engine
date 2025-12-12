#include "TreeGenerate.h"

#include "Logger/Log.h"
#include "Tree.h"
#include "TreeMeshBuilder.h"

GraphicsApi::MeshRawData GameEngine::generateTree(int attractorCount, float crownRadius, float crownHeight,
                                                  float attractionDistance, float killDistance, float segmentLength,
                                                  float trunkOffsetY)
{
    LOG_DEBUG << "Generate tree branches...";
    Tree tree{};
    tree.rootPosition = vec3(0, -3.f, 0);
    tree.prepareAttractors(attractorCount, crownRadius);
    auto status = tree.build();

    if (status and not status->empty())
    {
        LOG_WARN << status.value();
    }

    LOG_DEBUG << "Buildng tree mesh... ("
              << "Branches : " << tree.GetBranches().size() << ")";
    TreeMeshBuilder builder(tree.GetBranches());
    return builder.buildCylinderMesh();
}
