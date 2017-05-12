#include "OpenGLLoader.h"

void COpenGLLoader::AddObjectToOpenGLLoadingPass(COpenGLObject* obj)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_OpenGLPassLoad.push_back(obj);
}
COpenGLObject* COpenGLLoader::GetObjectToOpenGLLoadingPass()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	if (m_OpenGLPassLoad.empty())
		return nullptr;
	COpenGLObject *obj = m_OpenGLPassLoad.back();
	m_OpenGLPassLoad.pop_back();
	return obj;
}

void COpenGLLoader::AddObjectToOpenGLPostLoadingPass(COpenGLObject * obj)
{
	std::lock_guard<std::mutex> lock(m_Mutex2);
	m_OpenGLPostPassLoad.push_back(obj);
}

COpenGLObject * COpenGLLoader::GetObjectToOpenGLPostLoadingPass()
{
	std::lock_guard<std::mutex> lock(m_Mutex2);
	if (m_OpenGLPostPassLoad.empty())
		return nullptr;
	COpenGLObject *obj = m_OpenGLPostPassLoad.back();
	m_OpenGLPostPassLoad.pop_back();
	return obj;
}
