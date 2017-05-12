#pragma once
#include <list>
#include "OpenGLObject.h"
#include <thread>
#include <mutex>
class COpenGLLoader
{
public:
	void 			AddObjectToOpenGLLoadingPass(COpenGLObject* obj);
	COpenGLObject*	GetObjectToOpenGLLoadingPass();
	void 			AddObjectToOpenGLPostLoadingPass(COpenGLObject* obj);
	COpenGLObject*	GetObjectToOpenGLPostLoadingPass();
private:
	std::list<COpenGLObject*> m_OpenGLPassLoad;
	std::list<COpenGLObject*> m_OpenGLPostPassLoad;
	std::mutex m_Mutex;
	std::mutex m_Mutex2;
};