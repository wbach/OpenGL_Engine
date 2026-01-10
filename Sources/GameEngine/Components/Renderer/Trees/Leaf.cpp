#include "Leaf.h"

namespace GameEngine
{
LeafSSBO ConvertToSSBO(const Leaf& leaf)
{
    LeafSSBO ssbo;
    ssbo.positionAndSizeRandomness = glm::vec4(leaf.position, leaf.sizeRandomness);
    ssbo.direction = glm::vec4(leaf.direction, 0.0f);
    ssbo.colorRandomnessAndTextureIndex = glm::vec4(leaf.colorRandomness, static_cast<float>(leaf.textureIndex));

    return ssbo;
}
std::vector<LeafSSBO> PrepareSSBOData(const std::vector<Leaf>& cpuLeaves)
{
    std::vector<LeafSSBO> ssboData;
    ssboData.reserve(cpuLeaves.size());

    for (const auto& leaf : cpuLeaves)
    {
        ssboData.push_back(ConvertToSSBO(leaf));
    }
    return ssboData;
}
}  // namespace GameEngine
