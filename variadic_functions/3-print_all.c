#include <stdio.h>
#include <stdarg.h>
#include "variadic_functions.h"

/**
 * print_all - prints anything based on a format string
 * @format: list of types (c=char, i=int, f=float, s=string)
 */
void print_all(const char * const format, ...)
{
	va_list args;
	unsigned int i;
	char *sep;
	char c;
	int n;
	double f;
	char *s;

	va_start(args, format);
	i = 0;
	sep = "";
	while (format && format[i])
	{
		switch (format[i])
		{
			case 'c':
				c = va_arg(args, int);
				printf("%s%c", sep, c);
				sep = ", ";
				break;
			case 'i':
				n = va_arg(args, int);
				printf("%s%d", sep, n);
				sep = ", ";
				break;
			case 'f':
				f = va_arg(args, double);
				printf("%s%f", sep, f);
				sep = ", ";
				break;
			case 's':
				s = va_arg(args, char *);
				if (s == NULL)
					s = "(nil)";
				printf("%s%s", sep, s);
				sep = ", ";
				break;
			default:
				break;
		}
		i++;
	}
	va_end(args);
	printf("\n");
}
