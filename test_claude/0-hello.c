#include "main.h"
#include <unistd.h>

/**
 * hello - prints Hello Holberton
 *
 * Return: void
 */
void hello(void)
{
	write(1, "Hello Holberton\n", 16);
}
