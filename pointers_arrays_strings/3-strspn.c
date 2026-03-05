#include "main.h"

/**
 * _strspn - gets the length of a prefix substring
 * @s: pointer to string
 * @accept: pointer to bytes to accept
 *
 * Return: number of bytes in initial segment of s from accept
 */
unsigned int _strspn(char *s, char *accept)
{
	unsigned int count, i;
	int found;

	count = 0;
	while (*s != '\0')
	{
		found = 0;
		i = 0;
		while (accept[i] != '\0')
		{
			if (*s == accept[i])
			{
				found = 1;
				break;
			}
			i++;
		}
		if (found == 0)
			return (count);
		count++;
		s++;
	}
	return (count);
}
