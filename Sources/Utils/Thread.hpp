#pragma once
#ifdef USE_MINGW
#include <mingw.thread.h>
#include "UndifineWindows.h"
#else
#include <thread>
#endif