#include <iostream>
#include "../../cpputils/algo.h"
using namespace std;
using namespace spes::algo;

int main()
{
	int arr[] = {1, 3, 8, 6, 5, 0, 7, 3, 2};
	function<bool(const int&, const int&)> foo = [](const int& a, const int& b) {return a < b; };
	qsort((int*)arr, 9, foo);
	for (int a : arr)
	{
		cout << a << " ";
	}
	cout << endl;
	return 0;
}