#pragma once
#include <iostream>
#include <functional>
#include "math.h"

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

    enum CutRet
    {
        CR_ACCEPTED = 0,
        CR_REFUSED = 1,
    };
    enum CSBIT
    {
        CB_BELOW = 0x01,//0001
        CB_RIGHT = 0x02,//0010
        CB_ABOVE = 0x04,//0100
        CB_LEFT = 0x08,//1000

        CB_BELOW_INV = 0xfe,//1110
        CB_RIGHT_INV = 0xfd,//1101
        CB_ABOVE_INV = 0xfb,//1011
        CB_LEFT_INV = 0xf7,//
    };
    typedef u8 KEY;
    KEY GenKey(const spes::math::vector2d& p, const spes::math::rect& r);
    int cohen_sutherland(spes::math::line2d& line, const spes::math::rect& rect);

    int sutherland_hodgman(const spes::math::polygon2d& src, const spes::math::rect& bnds, spes::math::polygon2d& rslt);
    int sutherland_hodgman(const spes::math::polygon2d& src, const spes::math::polygon2d& bnds, spes::math::polygon2d& rslt);
    int weiler_atherton(const spes::math::polygon2d& src, const spes::math::polygon2d& bnds, spes::math::polygon2d& rslt);
}