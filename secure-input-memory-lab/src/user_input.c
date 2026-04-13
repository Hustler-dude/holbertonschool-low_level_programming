#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_input.h"

/**
 * read_username - reads a username from stdin safely
 *
 * Return: pointer to allocated string, or NULL on failure
 */
char *read_username(void)
{
	char buffer[32];
	char *username = NULL;
	size_t len;

	if (fgets(buffer, sizeof(buffer), stdin) == NULL)
		return (NULL);

	len = strlen(buffer);
	if (len > 0 && buffer[len - 1] == '\n')
		buffer[len - 1] = '\0';

	username = malloc(strlen(buffer) + 1);
	if (username == NULL)
		return (NULL);

	strcpy(username, buffer);
	return (username);
}
