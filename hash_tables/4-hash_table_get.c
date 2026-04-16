#include <string.h>
#include "hash_tables.h"

/**
 * hash_table_get - retrieves a value associated with a key
 * @ht: the hash table to look into
 * @key: the key to look for
 *
 * Return: the value associated with the key, or NULL if not found
 */
char *hash_table_get(const hash_table_t *ht, const char *key)
{
	hash_node_t *cur;
	unsigned long int idx;

	if (ht == NULL || key == NULL || *key == '\0')
		return (NULL);

	idx = key_index((const unsigned char *)key, ht->size);
	cur = ht->array[idx];
	while (cur != NULL)
	{
		if (strcmp(cur->key, key) == 0)
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}
