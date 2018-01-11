#pragma once
#ifdef USE_MINGW
#include <mingw.mutex.h>
#undef GetMessage
#include "UndifineWindows.h"
#else
#include <mutex>
#endif