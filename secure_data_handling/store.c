#include <stdlib.h>
#include <string.h>
#include "store.h"

/**
 * store_init - initialises an empty store
 * @st: store to initialise
 */
void store_init(store_t *st)
{
	if (st)
		st->head = NULL;
}

/**
 * store_add - inserts a session into the store
 * @st: target store
 * @s: session to insert
 *
 * Ownership: store_add always takes ownership of @s.
 * On success the store owns @s and frees it on delete/destroy.
 * On any failure store_add destroys @s so the caller never leaks it.
 *
 * Return: 1 on success, 0 on duplicate id, bad input, or allocation failure
 */
int store_add(store_t *st, session_t *s)
{
	node_t *n;
	node_t *cur;

	if (!st || !s || !s->id)
	{
		session_destroy(s);
		return (0);
	}

	cur = st->head;
	while (cur)
	{
		if (cur->sess && cur->sess->id &&
			strcmp(cur->sess->id, s->id) == 0)
		{
			session_destroy(s);
			return (0);
		}
		cur = cur->next;
	}

	n = (node_t *)malloc(sizeof(*n));
	if (!n)
	{
		session_destroy(s);
		return (0);
	}
	n->sess = s;
	n->next = st->head;
	st->head = n;
	return (1);
}

/**
 * store_get - retrieves a session by id without removing it
 * @st: store to search
 * @id: session identifier to look for
 *
 * Return: pointer to session (still owned by store), or NULL if not found
 */
session_t *store_get(store_t *st, const char *id)
{
	node_t *cur;

	if (!st || !id)
		return (NULL);

	cur = st->head;
	while (cur)
	{
		if (cur->sess && cur->sess->id &&
			strcmp(cur->sess->id, id) == 0)
			return (cur->sess);
		cur = cur->next;
	}
	return (NULL);
}

/**
 * store_delete - removes a session from the store by id
 * @st: store to modify
 * @id: session identifier
 * @out: if non-NULL, ownership is transferred to caller;
 *       if NULL, the session is destroyed by this function
 *
 * Return: 1 if deleted, 0 if not found
 */
int store_delete(store_t *st, const char *id, session_t **out)
{
	node_t *cur;
	node_t *prev;

	if (!st || !id)
		return (0);

	prev = NULL;
	cur = st->head;
	while (cur)
	{
		if (cur->sess && cur->sess->id &&
			strcmp(cur->sess->id, id) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				st->head = cur->next;

			if (out)
				*out = cur->sess;
			else
				session_destroy(cur->sess);

			free(cur);
			return (1);
		}
		prev = cur;
		cur = cur->next;
	}
	return (0);
}

/**
 * store_destroy - frees all sessions and nodes in the store
 * @st: store to empty
 */
void store_destroy(store_t *st)
{
	node_t *cur;
	node_t *next;

	if (!st)
		return;

	cur = st->head;
	while (cur)
	{
		next = cur->next;
		session_destroy(cur->sess);
		free(cur);
		cur = next;
	}
	st->head = NULL;
}
