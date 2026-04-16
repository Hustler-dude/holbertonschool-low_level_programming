# Doubly Linked Lists

Implementation of doubly linked list operations in C.

## Data Structure

```c
typedef struct dlistint_s
{
    int n;
    struct dlistint_s *prev;
    struct dlistint_s *next;
} dlistint_t;
```

## Files

| File | Description |
|------|-------------|
| `lists.h` | Header file with struct definition and prototypes |
| `0-print_dlistint.c` | Print all elements of a doubly linked list |
