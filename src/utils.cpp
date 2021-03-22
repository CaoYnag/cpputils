#include <spes/utils.h>
#ifdef __unix__
#include <sys/time.h>
#elif WIN32
#include <Windows.h>
#endif

long cur_ms()
{
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#elif WIN32
    return GetTickCount();
#endif
}