#pragma once
#ifdef USE_WINDOWS_THREADS
#include <mingw.thread.h>
#include <mingw.mutex.h>
#else
#include <thread>
#include <mutex>
#endif