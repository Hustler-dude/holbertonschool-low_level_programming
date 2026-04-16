#include <stdlib.h>
#include <string.h>
#include "lists.h"

/**
 * add_node - adds a new node at the beginning of a list_t list
 * @head: pointer to pointer to the head of the list
 * @str: string to store in the new node
 *
 * Return: address of the new node, or NULL if it failed
 */
list_t *add_node(list_t **head, const char *str)
{
	list_t *new;
	unsigned int len;
	const char *p;

	new = malloc(sizeof(list_t));
	if (new == NULL)
		return (NULL);
	new->str = strdup(str);
	if (new->str == NULL)
	{
		free(new);
		return (NULL);
	}
	for (p = str, len = 0; *p != '\0'; p++)
		len++;
	new->len = len;
	new->next = *head;
	*head = new;
	return (new);
}
