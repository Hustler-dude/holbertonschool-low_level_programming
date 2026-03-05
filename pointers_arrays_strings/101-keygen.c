#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * main - generates random valid passwords for 101-crackme
 *
 * Return: Always 0
 */
int main(void)
{
	int sum, i;
	char c;

	srand(time(NULL));
	sum = 0;

	while (sum <= 2645)
	{
		c = rand() % 128;
		if (sum + c > 2772)
			break;
		putchar(c);
		sum += c;
	}
	putchar(2772 - sum);
	return (0);
}
