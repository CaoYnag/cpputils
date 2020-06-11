#pragma once
#include <vector>
#include <cmath>

namespace spes::math
{
	template<typename T>
	double avg(const std::vector<T>& nums)
	{
		if (nums.size() <= 0) return .0;
		T sum = nums[0];
		for (int i = 1; i < nums.size(); ++i)
			sum += nums[i];
		return (sum + .0) / nums.size();
	}

	template<typename T>
	double variance(const std::vector<T>& nums)
	{
		if (nums.size() <= 1) return .0;
		return variance(nums, avg(nums));
	}

	template<typename T>
	double variance(const std::vector<T>& nums, double avg)
	{
		if (nums.size() <= 1) return .0;
		double sum = .0;

		for (T i : nums)
		{
			double d = i - avg;
			sum += d * d;
		}
		return sum / nums.size();
	}

	template<typename T>
	double standard_deviation(const std::vector<T>& nums)
	{
		return sqrt(variance(nums));
	}
	template<typename T>
	double standard_deviation(const std::vector<T>& nums, double avg)
	{
		return sqrt(variance(nums, avg));
	}
}