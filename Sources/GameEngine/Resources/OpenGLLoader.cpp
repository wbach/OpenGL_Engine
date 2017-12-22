#include "OpenGLLoader.h"

void COpenGLLoader::AddObjectToOpenGLLoadingPass(COpenGLObject* obj)
{
    std::lock_guard<std::mutex> lock(mutex);
    openGLPassLoad.push_back(obj);
}
COpenGLObject* COpenGLLoader::GetObjectToOpenGLLoadingPass()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (openGLPassLoad.empty())
		return nullptr;
    COpenGLObject *obj = openGLPassLoad.back();
    openGLPassLoad.pop_back();
	return obj;
}

void COpenGLLoader::AddObjectToOpenGLPostLoadingPass(COpenGLObject * obj)
{
    std::lock_guard<std::mutex> lock(mutex2);
    openGLPostPassLoad.push_back(obj);
}

COpenGLObject * COpenGLLoader::GetObjectToOpenGLPostLoadingPass()
{
    std::lock_guard<std::mutex> lock(mutex2);
    if (openGLPostPassLoad.empty())
		return nullptr;
    COpenGLObject *obj = openGLPostPassLoad.back();
    openGLPostPassLoad.pop_back();
	return obj;
}
