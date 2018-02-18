#include "ModelFactory.h"
#include "../../Resources/ResourceManager.h"

CModel* GameEngine::LoadModel(CResourceManager* manager, const std::string& filename)
{
	// manager ownership
	ModelRawPtr m = manager->LoadModel(filename);
	manager->GetOpenGlLoader().AddObjectToOpenGLLoadingPass(m);
	m->CalculateBoudnigBox();
	return m;
}
