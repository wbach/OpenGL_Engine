#include "ModelFactory.h"
#include "../../Resources/ResourceManager.h"

CModel* GameEngine::LoadModel(CResourceManager* manager, const vec3& normalizedScale, const std::string& filename)
{
	// manager ownership
	ModelRawPtr m = manager->LoadModel(filename);
	manager->GetOpenGlLoader().AddObjectToOpenGLLoadingPass(m);
	m->CalculateBoudnigBox();
	m->SetNormalizedScaleVector(normalizedScale.x, normalizedScale.y, normalizedScale.z);
	return m;
}
