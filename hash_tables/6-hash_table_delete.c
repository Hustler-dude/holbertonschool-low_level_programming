#include <stdlib.h>
#include "hash_tables.h"

/**
 * hash_table_delete - deletes a hash table
 * @ht: the hash table to delete
 */
void hash_table_delete(hash_table_t *ht)
{
	hash_node_t *cur;
	hash_node_t *tmp;
	unsigned long int i;

	if (ht == NULL)
		return;

	for (i = 0; i < ht->size; i++)
	{
		cur = ht->array[i];
		while (cur != NULL)
		{
			tmp = cur->next;
			free(cur->key);
			free(cur->value);
			free(cur);
			cur = tmp;
		}
	}
	free(ht->array);
	free(ht);
}
