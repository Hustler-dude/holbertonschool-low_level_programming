#include <stdio.h>
#include "hash_tables.h"

/**
 * hash_table_print - prints a hash table
 * @ht: the hash table to print
 */
void hash_table_print(const hash_table_t *ht)
{
	hash_node_t *cur;
	unsigned long int i;
	int first;

	if (ht == NULL)
		return;

	first = 1;
	printf("{");
	for (i = 0; i < ht->size; i++)
	{
		cur = ht->array[i];
		while (cur != NULL)
		{
			if (!first)
				printf(", ");
			printf("'%s': '%s'", cur->key, cur->value);
			first = 0;
			cur = cur->next;
		}
	}
	printf("}\n");
}
