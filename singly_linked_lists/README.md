# Singly Linked Lists

Implementation of singly linked list operations in C.

## Data Structure

```c
typedef struct list_s
{
    char *str;
    unsigned int len;
    struct list_s *next;
} list_t;
```

## Files

| File | Description |
|------|-------------|
| `lists.h` | Header file with struct definition and prototypes |
| `0-print_list.c` | Print all elements of a list |
