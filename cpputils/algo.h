#pragma once
#include <iostream>
#include <functional>

namespace spes::algo
{
	template<class T>
	void showArr(T* arr, int size)
	{
		for (int i = 0; i < size; ++i)
		{
			std::cout << arr[i] << " ";
		}
		std::cout << std::endl;
	}
	template<class T>
	void swap(T* arr, int p, int q)
	{
		if (p == q || p < 0 || q < 0) return;
		T tmp = arr[p];
		arr[p] = arr[q];
		arr[q] = tmp;
	}
	/*
	 * Quick Sort
	 * if the cmp function return true, then first param of it will
	 * be placed in front of another.
	*/

	template<class T>
	void qsort(T* arr, int size, std::function<bool(const T&, const T&)> cmp)
	{
		if (size <= 1) return;
		constexpr int BUCKET = 0;
		int p = 0, q = 0;
		for (int i = 1; i < size; ++i)
		{
			if (cmp(arr[i], arr[BUCKET]))
			{
				swap(arr, ++p, i);
				++q;
			}
			else
				++q;
		}
		swap(arr, 0, p);
		qsort(arr, p, cmp);
		qsort(arr + p + 1, size - p - 1, cmp);
	}
}