#include "ModelFactory.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/OpenGLLoader.h"

namespace GameEngine
{
CModel* LoadModel(IResourceManager* manager, const std::string& filename)
{
    // manager ownership
    ModelRawPtr m = manager->LoadModel(filename);
    manager->GetOpenGlLoader().AddObjectToOpenGLLoadingPass(m);
    return m;
}
} // namespace GamEngine
