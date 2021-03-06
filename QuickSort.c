// QuickSort.c : Defines the entry point for the console application.
//

#include <stdio.h>  
#include <omp.h>
#include <time.h>

void swap(int * a, int * b);
int partition(int arr[], int low, int high);
void quickSort(int arr[], int low, int high);
int parallelpartition(int arr[], int low, int high);
void parallelquickSort(int arr[], int low, int high, int threadCount);
#define numberCount 1600
//References:
//Quicksort found at: https://www.geeksforgeeks.org/quick-sort/

int main()
{
	int threadCount = 2;
	static int quicksortArray[numberCount];
	static int parallelquicksortArray[numberCount];
	//create two random arrays with 100000 numbers anywhere from 0-15000000
	for (int i = 0; i < numberCount; i++)
	{
		int randomInt = rand() % 15000000;
		quicksortArray[i] = randomInt;
		parallelquicksortArray[i] = randomInt;
	}
	//Run quick sort and time (not parallelized)
	clock_t qsortbegin = clock();
	quickSort(quicksortArray, 0, numberCount - 1);
	clock_t qsortend = clock();
	double qsorttime_spent = (double)(qsortend - qsortbegin) / CLOCKS_PER_SEC;
	printf("Quick Sort took: %.10lf seconds\n", qsorttime_spent);

	//Run parallelized version of quick sort and time
	double parallelqsortbegin = omp_get_wtime();
	parallelquickSort(parallelquicksortArray, 0, numberCount - 1, threadCount);
	double parallelqsortend = omp_get_wtime();
	double parallelqsorttime_spent = (double)(parallelqsortend - parallelqsortbegin) / CLOCKS_PER_SEC;
	printf("Parallelized Quick Sort took: %f seconds\n", parallelqsorttime_spent);

    return 0;
}

// A utility function to swap two elements
void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

///*** This is the unmodified quicksort - not parallel

/* This function takes last element as pivot, places
the pivot element at its correct position in sorted
array, and places all smaller (smaller than pivot)
to left of pivot and all greater elements to right
of pivot */
int partition(int arr[], int low, int high)
{
	int pivot = arr[high];    // pivot
	int i = (low - 1);  // Index of smaller element

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or
		// equal to pivot
		if (arr[j] <= pivot)
		{
			i++;    // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low  --> Starting index,
high  --> Ending index */
void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		at right place */
		int pi = partition(arr, low, high);

		// Separately sort elements before
		// partition and after partition
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}


///*** This is the modified quicksort - parallel
/* This function takes last element as pivot, places
the pivot element at its correct position in sorted
array, and places all smaller (smaller than pivot)
to left of pivot and all greater elements to right
of pivot */
int parallelpartition(int arr[], int low, int high, int thread_count)
{
	int pivot = arr[high];    // pivot
	int i = (low - 1);  // Index of smaller element
	int j;
#pragma omp parallel for num_threads(thread_count) \
   default(none) private(j) shared(arr, thread_count, pivot, i)
	for(j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or
		// equal to pivot
		if (arr[j] <= pivot)
		{
			i++;    // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low  --> Starting index,
high  --> Ending index */
void parallelquickSort(int arr[], int low, int high, int threadCount)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		at right place */
		int pi = parallelpartition(arr, low, high, threadCount);
#pragma omp parallel sections
		{
			// Separately sort elements before
			// partition and after partition
#pragma omp section
			parallelquickSort(arr, low, pi - 1, threadCount);
#pragma omp section
			parallelquickSort(arr, pi + 1, high, threadCount);
		}
	}
}