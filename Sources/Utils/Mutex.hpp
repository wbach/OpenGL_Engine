#pragma once
#ifdef USE_WINDOWS_THREADS
#include <mingw.mutex.h>
#else
#include <mutex>
#endif