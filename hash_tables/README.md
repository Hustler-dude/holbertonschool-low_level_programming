# Hash Tables

Implementation of hash table operations in C using chaining for collision handling.

## Data Structures

```c
typedef struct hash_node_s
{
    char *key;
    char *value;
    struct hash_node_s *next;
} hash_node_t;

typedef struct hash_table_s
{
    unsigned long int size;
    hash_node_t **array;
} hash_table_t;
```

## Files

| File | Description |
|------|-------------|
| `hash_tables.h` | Header file with struct definitions and prototypes |
| `0-hash_table_create.c` | Create a hash table |
