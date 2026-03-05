#include "main.h"

/**
 * cap_string - capitalizes all words of a string
 * @s: string to modify
 *
 * Return: pointer to modified string
 */
char *cap_string(char *s)
{
	int i, j;
	char separators[] = " \t\n,;.!?\"(){}";

	i = 0;
	if (s[i] >= 'a' && s[i] <= 'z')
		s[i] = s[i] - 32;
	i++;
	while (s[i] != '\0')
	{
		j = 0;
		while (separators[j] != '\0')
		{
			if (s[i - 1] == separators[j])
			{
				if (s[i] >= 'a' && s[i] <= 'z')
					s[i] = s[i] - 32;
			}
			j++;
		}
		i++;
	}
	return (s);
}
