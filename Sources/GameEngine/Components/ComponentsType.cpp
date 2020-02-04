#include "ComponentsTypes.h"

#define RETURN_STR(x) case ComponentsType::x : return(#x)

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
} // namespace std
#undef RETURN_STR
