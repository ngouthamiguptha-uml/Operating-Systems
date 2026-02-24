#ifndef LIST_H
#define LIST_H

// Opaque list type - both cll_global and cll_hoh file define struct list differently
typedef struct list list_t;

// Create a new empty list 
list_t *list_create(void);

// Destroy list and free all nodes + internal locks + list itself
void list_destroy(list_t *list);

// Insert key in sorted order (ascending)
int list_insert(list_t *list, int key);

// Delete key if present
int list_delete(list_t *list, int key);

// Lookup key (0 if found, -1 if not found)
int list_lookup(list_t *list, int key);

#endif