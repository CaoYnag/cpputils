#include <spes/utils.h>
#ifdef __unix__
#include <sys/time.h>
#elif WIN32
#include <Windows.h>
#endif
using namespace std;

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

string str_uppercase(const string& s)
{
	string ret(s);
	char* p = (char*)ret.c_str();
	for(int i = 0; i < ret.size(); ++i)
		if(p[i] <= 'z' && p[i] >= 'a') p[i] -= 32;

	return ret;
}
string str_lowercase(const string& s)
{
	string ret(s);
	char* p = (char*)ret.c_str();
	for(int i = 0; i < ret.size(); ++i)
		if(p[i] <= 'Z' && p[i] >= 'A') p[i] |= 0x20;

	return ret;
}

static uint32_t _smush_seed = 0, _smush_idx = 0;
uint32_t smush()
{
	constexpr const uint32_t m = 0x5bd1e995;
	uint32_t k = _smush_idx++ * m;
	_smush_seed = (k ^ (k >> 24) ^ (_smush_seed * m)) * m;
	return _smush_seed ^ (_smush_seed >> 13);
}

void smush_seed(uint32_t s)
{
	_smush_seed = s;
	_smush_idx = 0;
}
