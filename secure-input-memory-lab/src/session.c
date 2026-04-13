#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "session.h"

/**
 * session_create - creates a new session with a copy of username
 * @username: the username string to store
 *
 * Return: pointer to new session, or NULL on failure
 */
session_t *session_create(char *username)
{
	session_t *session = NULL;

	session = malloc(sizeof(session_t));
	if (session == NULL)
		return (NULL);

	session->user = malloc(strlen(username) + 1);
	if (session->user == NULL)
	{
		free(session);
		return (NULL);
	}
	strcpy(session->user, username);
	session->access_level = 1;

	return (session);
}

void session_print(session_t *session)
{
    if (session == NULL)
        return;

    printf("User: %s\n", session->user);
    printf("Access level: %d\n", session->access_level);
}

void session_destroy(session_t *session)
{
    if (session == NULL)
        return;

    free(session->user);
    free(session);
}
