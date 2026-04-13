#include <stdio.h>
#include <stdlib.h>
#include "session.h"

session_t *session_create(char *username)
{
    session_t *session = NULL;

    session = malloc(sizeof(session_t));
    if (session == NULL)
        return NULL;

    session->user = username;
    session->access_level = 1;

    return session;
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
