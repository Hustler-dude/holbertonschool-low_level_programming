#ifndef SESSION_H
#define SESSION_H

typedef struct session
{
    char *user;
    int access_level;
} session_t;

session_t *session_create(char *username);
void session_print(session_t *session);
void session_destroy(session_t *session);

#endif
