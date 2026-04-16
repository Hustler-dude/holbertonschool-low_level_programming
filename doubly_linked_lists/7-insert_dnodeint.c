#include <stdlib.h>
#include "lists.h"

/**
 * insert_dnodeint_at_index - inserts a new node at a given position
 * @h: pointer to pointer to the head of the list
 * @idx: index where the new node should be added (starts at 0)
 * @n: integer value for the new node
 *
 * Return: address of the new node, or NULL if it failed
 */
dlistint_t *insert_dnodeint_at_index(dlistint_t **h, unsigned int idx, int n)
{
	dlistint_t *new;
	dlistint_t *cur;
	unsigned int i;

	if (idx == 0)
		return (add_dnodeint(h, n));

	cur = *h;
	for (i = 0; i < idx - 1; i++)
	{
		if (cur == NULL)
			return (NULL);
		cur = cur->next;
	}
	if (cur == NULL)
		return (NULL);
	if (cur->next == NULL)
		return (add_dnodeint_end(h, n));

	new = malloc(sizeof(dlistint_t));
	if (new == NULL)
		return (NULL);
	new->n = n;
	new->next = cur->next;
	new->prev = cur;
	cur->next->prev = new;
	cur->next = new;
	return (new);
}
