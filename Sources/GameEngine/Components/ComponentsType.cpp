#include "ComponentsTypes.h"

#define RETURN_STR(x) case ComponentsType::x : return(#x)
#define FROM_STR(x) if (name == #x) return ComponentsType::x

using namespace GameEngine::Components;

namespace std
{
std::string to_string(GameEngine::Components::ComponentsType type)
{
    switch (type)
    {
       RETURN_STR(Animator);
       RETURN_STR(BoxShape);
       RETURN_STR(MeshShape);
       RETURN_STR(SphereShape);
       RETURN_STR(TerrainShape);
       RETURN_STR(CapsuleShape);
       RETURN_STR(Rigidbody);
       RETURN_STR(Renderer);
       RETURN_STR(TreeRenderer);
       RETURN_STR(ParticleEffect);
       RETURN_STR(SkyBox);
       RETURN_STR(Skydome);
       RETURN_STR(Grass);
       RETURN_STR(TerrainRenderer);
       RETURN_STR(TerrainMeshRenderer);
       RETURN_STR(Water);
       RETURN_STR(ThridPersonCamera);
       RETURN_STR(PlayerInputController);
       RETURN_STR(CharacterController);
    }
   return std::to_string(static_cast<int>(type));
}
std::optional<GameEngine::Components::ComponentsType> from_string(const std::string& name)
{
    FROM_STR(Animator);
    FROM_STR(BoxShape);
    FROM_STR(MeshShape);
    FROM_STR(SphereShape);
    FROM_STR(TerrainShape);
    FROM_STR(CapsuleShape);
    FROM_STR(Rigidbody);
    FROM_STR(Renderer);
    FROM_STR(TreeRenderer);
    FROM_STR(ParticleEffect);
    FROM_STR(SkyBox);
    FROM_STR(Skydome);
    FROM_STR(Grass);
    FROM_STR(TerrainRenderer);
    FROM_STR(TerrainMeshRenderer);
    FROM_STR(Water);
    FROM_STR(ThridPersonCamera);
    FROM_STR(PlayerInputController);
    FROM_STR(CharacterController);

    return {};
}
} // namespace std
#undef RETURN_STR
#undef FROM_STR
