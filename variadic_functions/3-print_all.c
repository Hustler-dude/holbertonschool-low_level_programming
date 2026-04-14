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
	unsigned int i = 0;
	char *sep = "";
	char *s;

	va_start(args, format);
	while (format && format[i])
	{
		switch (format[i++])
		{
		case 'c':
			printf("%s%c", sep, va_arg(args, int));
			sep = ", ";
			break;
		case 'i':
			printf("%s%d", sep, va_arg(args, int));
			sep = ", ";
			break;
		case 'f':
			printf("%s%f", sep, va_arg(args, double));
			sep = ", ";
			break;
		case 's':
			s = va_arg(args, char *);
			if (s == NULL)
				s = "(nil)";
			printf("%s%s", sep, s);
			sep = ", ";
			break;
		}
	}
	va_end(args);
	printf("\n");
}
