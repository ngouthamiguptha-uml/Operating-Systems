#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

//Node Definition (GLOBAL LOCK)
typedef struct node {
    int key;
    struct node *next;
} node_t;


//List Definition (GLOBAL LOCK) - One mutex protects the entire list
typedef struct list {
    node_t *head;
    pthread_mutex_t lock;
} list_t;

//Create / Destroy
// Allocate and initialize an empty list
list_t *list_create(void) {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (!list) return NULL;

    list->head = NULL;
    pthread_mutex_init(&list->lock, NULL);
    return list;
}

// Free all nodes and destroy the list lock
void list_destroy(list_t *list) {
    if (!list) return;

    // Lock the list while freeing nodes
    pthread_mutex_lock(&list->lock);

    node_t *curr = list->head;
    while (curr) {
        node_t *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    list->head = NULL;

    pthread_mutex_unlock(&list->lock);

    // Now destroy the lock and free the list object
    pthread_mutex_destroy(&list->lock);
    free(list);
}

//Insert (sorted)
int list_insert(list_t *list, int key) {
    pthread_mutex_lock(&list->lock);

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node) {
        pthread_mutex_unlock(&list->lock);
        return -1;
    }

    new_node->key = key;
    new_node->next = NULL;

    // Insert at head (empty list OR smallest key)
    if (list->head == NULL || list->head->key >= key) {
        new_node->next = list->head;
        list->head = new_node;
        pthread_mutex_unlock(&list->lock);
        return 0;
    }

    // Find insertion point
    node_t *curr = list->head;
    while (curr->next && curr->next->key < key) {
        curr = curr->next;
    }

    new_node->next = curr->next;
    curr->next = new_node;

    pthread_mutex_unlock(&list->lock);
    return 0;
}

//Delete key if present
int list_delete(list_t *list, int key) {
    pthread_mutex_lock(&list->lock);

    node_t *curr = list->head;
    node_t *prev = NULL;

    while (curr) {
        if (curr->key == key) {
            // Remove curr from list
            if (prev) prev->next = curr->next;
            else      list->head = curr->next;

            free(curr);
            pthread_mutex_unlock(&list->lock);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&list->lock);
    return -1; // not found
}


//Lookup
int list_lookup(list_t *list, int key) {
    pthread_mutex_lock(&list->lock);

    node_t *curr = list->head;
    while (curr) {
        if (curr->key == key) {
            pthread_mutex_unlock(&list->lock);
            return 0;
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&list->lock);
    return -1; // not found
}