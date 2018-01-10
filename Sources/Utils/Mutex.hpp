#pragma once
#ifdef USE_MINGW
#include <mingw.mutex.h>
#else
#include <mutex>
#endif