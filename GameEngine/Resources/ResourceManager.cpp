#include "ResourceManager.h"

CResourceManager::CResourceManager()
	:  m_TextureLoader(m_Textures, m_OpenGlLoader)
{
}

CModel * CResourceManager::LoadModel(const std::string & file)
{

	for (const auto& model : m_Models)
	{
		if (model->GetFileName().compare(file) == 0)
			return model.get();
	}
	//CAssimModel  CMyModel
	m_Models.emplace_back(new CMyModel(m_TextureLoader));
    m_Models.back()->InitModel(file);
	return m_Models.back().get();
}
