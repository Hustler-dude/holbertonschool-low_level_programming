#include <stdarg.h>
#include "variadic_functions.h"

/**
 * sum_them_all - returns the sum of all parameters
 * @n: number of parameters
 *
 * Return: sum of all parameters, or 0 if n == 0
 */
int sum_them_all(const unsigned int n, ...)
{
	unsigned int i;
	int sum;
	va_list args;

	if (n == 0)
		return (0);

	va_start(args, n);
	sum = 0;
	for (i = 0; i < n; i++)
		sum += va_arg(args, int);
	va_end(args);

	return (sum);
}
