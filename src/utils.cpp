#include <spes/utils.h>
#ifdef __unix__
#include <sys/time.h>
#elif WIN32
#endif

long cur_ms()
{
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#elif WIN32
#endif
}