#include <stdio.h>
#include <stdlib.h>
#include "user_input.h"
#include "session.h"

int main(void)
{
    char *username = NULL;
    session_t *session = NULL;

    printf("Enter username: ");
    username = read_username();

    if (username == NULL)
    {
        printf("Failed to read username\n");
        return 1;
    }

    session = session_create(username);
    if (session == NULL)
    {
        printf("Failed to create session\n");
        free(username);
        return 1;
    }

    session_print(session);
    session_destroy(session);

    printf("Goodbye %s\n", username);

    free(username);
    return 0;
}
