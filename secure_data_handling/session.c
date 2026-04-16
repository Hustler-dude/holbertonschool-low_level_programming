#include <stdlib.h>
#include <string.h>
#include "session.h"

/**
 * session_create - allocates and initialises a new session
 * @id: session identifier string (will be duplicated)
 * @uid: user identifier
 * @data: raw data bytes to copy (may be NULL if data_len is 0)
 * @data_len: number of bytes in @data
 *
 * Return: pointer to new session, or NULL on failure
 */
session_t *session_create(const char *id, unsigned int uid,
	const unsigned char *data, size_t data_len)
{
	session_t *s;

	s = (session_t *)malloc(sizeof(*s));
	if (!s)
		return (NULL);

	/* Duplicate id: the caller's buffer may be reused after this call */
	s->id = strdup(id);
	if (!s->id)
	{
		free(s);
		return (NULL);
	}

	s->uid = uid;

	if (data_len > 0)
	{
		s->data = (unsigned char *)malloc(data_len);
		if (!s->data)
		{
			/* Clean up already-allocated members before returning NULL */
			free(s->id);
			free(s);
			return (NULL);
		}
		memcpy(s->data, data, data_len);
		s->data_len = data_len;
	}
	else
	{
		s->data = NULL;
		s->data_len = 0;
	}

	return (s);
}

/**
 * session_set_data - replaces the data buffer of a session
 * @s: target session
 * @data: new data bytes
 * @data_len: number of bytes in @data
 *
 * Return: 1 on success, 0 on failure (session data unchanged on failure)
 */
int session_set_data(session_t *s, const unsigned char *data, size_t data_len)
{
	unsigned char *tmp;

	if (!s)
		return (0);

	if (data_len == 0)
	{
		free(s->data);
		s->data = NULL;
		s->data_len = 0;
		return (1);
	}

	/* Use tmp so original s->data is not lost if realloc fails */
	tmp = (unsigned char *)realloc(s->data, data_len);
	if (!tmp)
		return (0);

	s->data = tmp;
	memcpy(s->data, data, data_len);
	s->data_len = data_len;
	return (1);
}

/**
 * session_destroy - frees all memory owned by a session
 * @s: session to destroy
 */
void session_destroy(session_t *s)
{
	if (!s)
		return;

	free(s->id);
	free(s->data);
	free(s);
}
