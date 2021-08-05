#include <iostream>
#include <spes/algo.h>
#include <spes/utils.h>
#include <functional>
using namespace std;
using namespace spes;
using namespace spes::algo;

long bench(int batch, function<void()> foo)
{
	long s = cur_ms();

	for(int i = 0; i < batch; ++i)
		foo();

	long e = cur_ms();
	return e - s;
}

void bench()
{}

int main(int argc, char** argv)
{

	return 0;
}