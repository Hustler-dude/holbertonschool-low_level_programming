#include "main.h"

/**
 * _strstr - locates a substring
 * @haystack: pointer to string to search in
 * @needle: pointer to substring to find
 *
 * Return: pointer to beginning of substring, or NULL if not found
 */
char *_strstr(char *haystack, char *needle)
{
	int i;

	if (*needle == '\0')
		return (haystack);
	while (*haystack != '\0')
	{
		i = 0;
		while (needle[i] != '\0' && haystack[i] == needle[i])
			i++;
		if (needle[i] == '\0')
			return (haystack);
		haystack++;
	}
	return (0);
}
