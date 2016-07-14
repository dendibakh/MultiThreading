#pragma once

#include <utility>
#include <vector>
#include <cstddef>
#include <thread>

namespace single
{
	template <class T>
	static void merge(T *arr, size_t begin, size_t middle, size_t end)
	{
	   if (begin >= end || middle < begin || middle > end)
		  return;

	   if ( end == begin + 1 )
	   {
		  if (arr[begin] > arr[end])
			 std::swap(arr[begin], arr[end]);
		  return;
	   }

	   size_t arr_size = end + 1 - begin;
	   T *tmp = new T[arr_size];
	   for (size_t i = 0, k = begin ; i < arr_size; i++, k++)
		  tmp[i] = arr[k];

	   for(size_t i = begin, j = 0, k = middle - begin + 1; i <= end; i++)
	   {
		   if(j > middle - begin)
			  arr[i] = tmp[k++];
		   else if(k > end - begin)
			  arr[i] = tmp[j++];
		   else
			  arr[i] = (tmp[j] < tmp[k]) ? tmp[j++] : tmp[k++];
	   }
	   delete [] tmp;
	}

	template <class T>
	static void MergeSort(T *arr, size_t begin, size_t end)
	{
	   if( begin >= end)
		  return;

	   size_t middle = (begin + end) / 2;

	   MergeSort(arr, begin, middle);
	   MergeSort(arr, middle + 1, end);

	   merge(arr, begin, middle, end);
	}

	template <class T>
	void MergeSort(T *arr, int size)
	{
	   MergeSort(arr, 0, size - 1);
	}

	template <class T>
	void MergeSort(std::vector<T>& vect)
	{
	   MergeSort(vect.data(), vect.size());
	}
}

namespace parallel
{
	template <class T>
	static void merge(T *arr, size_t begin, size_t middle, size_t end)
	{
	   if (begin >= end || middle < begin || middle > end)
		  return;

	   if ( end == begin + 1 )
	   {
		  if (arr[begin] > arr[end])
			 std::swap(arr[begin], arr[end]);
		  return;
	   }

	   size_t arr_size = end + 1 - begin;
	   T *tmp = new T[arr_size];
	   for (size_t i = 0, k = begin ; i < arr_size; i++, k++)
		  tmp[i] = arr[k];

	   for(size_t i = begin, j = 0, k = middle - begin + 1; i <= end; i++)
	   {
		   if(j > middle - begin)
			  arr[i] = tmp[k++];
		   else if(k > end - begin)
			  arr[i] = tmp[j++];
		   else
			  arr[i] = (tmp[j] < tmp[k]) ? tmp[j++] : tmp[k++];
	   }
	   delete [] tmp;
	}

	template <class T>
	static void MergeSort2(T *arr, size_t begin, size_t end)
	{
	   if( begin >= end)
		  return;

	   size_t middle = (begin + end) / 2;

	   MergeSort2(arr, begin, middle);
	   MergeSort2(arr, middle + 1, end);

	   merge(arr, begin, middle, end);
	}

	template <class T>
	static void MergeSort(T *arr, size_t begin, size_t end)
	{
	   if( begin >= end)
		  return;

	   size_t middle = (begin + end) / 2;

	   std::thread th1([arr, begin, middle](){MergeSort2(arr, begin, middle);});
	   std::thread th2([arr, middle, end](){MergeSort2(arr, middle + 1, end);});
	   th1.join();
	   th2.join();

	   merge(arr, begin, middle, end);
	}

	template <class T>
	void MergeSort(T *arr, int size)
	{
	   MergeSort(arr, 0, size - 1);
	}

	template <class T>
	void MergeSort(std::vector<T>& vect)
	{
	   MergeSort(vect.data(), vect.size());
	}
}
