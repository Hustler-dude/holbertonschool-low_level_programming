#include "function_pointers.h"

/**
 * int_index - searches for an integer in an array
 * @array: the array of integers
 * @size: the number of elements in the array
 * @cmp: pointer to the comparison function
 *
 * Return: index of first match, or -1 if none found
 */
int int_index(int *array, int size, int (*cmp)(int))
{
	int i;

	if (size <= 0 || array == NULL || cmp == NULL)
		return (-1);
	for (i = 0; i < size; i++)
	{
		if (cmp(array[i]) != 0)
			return (i);
	}
	return (-1);
}
