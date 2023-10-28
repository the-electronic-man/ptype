#pragma once
#include <vector>

template <typename T>
bool Match(T item, std::vector<T> v)
{
	for (size_t i = 0; i < v.size(); i++)
	{
		if (item == v[i])
		{
			return true;
		}
	}
	return false;
}