#pragma once
#ifdef USE_WINDOWS_THREADS
#include <mingw.thread.h>
#else
#include <thread>
#endif