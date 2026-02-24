#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"

//Node Definition (HOH) - Each node has its own lock
typedef struct node {
    int key;
    struct node *next;
    pthread_mutex_t lock;
} node_t;


//List Definition (HOH) - List itself has only a head pointer and Synchronization is via node locks
typedef struct list {
    node_t *head;
    pthread_mutex_t head_lock; // protects access to head pointer to avoid race conditions
} list_t;


//Create / Destroy
// Allocate and initialize an empty list
list_t *list_create(void) {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (!list) return NULL;

    list->head = NULL;
    pthread_mutex_init(&list->head_lock, NULL);
    return list;
}

// Destroy all nodes (including their locks) and free the list
void list_destroy(list_t *list) {
    if (!list) return;

    pthread_mutex_lock(&list->head_lock);

    node_t *curr = list->head;
    while (curr) {
        node_t *tmp = curr;
        curr = curr->next;

        // Each node has a lock that must be destroyed before freeing
        pthread_mutex_destroy(&tmp->lock);
        free(tmp);
    }

    pthread_mutex_unlock(&list->head_lock);
    pthread_mutex_destroy(&list->head_lock);

    free(list);
}


//Insert (sorted) - Hand-over-hand locking
int list_insert(list_t *list, int key) {

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node) return -1;

    new_node->key = key;
    new_node->next = NULL;
    pthread_mutex_init(&new_node->lock, NULL);

    // Protect head pointer
    pthread_mutex_lock(&list->head_lock);

    // Empty list: just insert
    if (list->head == NULL) {
        list->head = new_node;
        pthread_mutex_unlock(&list->head_lock);
        return 0;
    }

    node_t *curr = list->head;
    pthread_mutex_lock(&curr->lock);
    pthread_mutex_unlock(&list->head_lock);

    // Insert before current head
    if (key < curr->key) {
        pthread_mutex_lock(&list->head_lock);
        new_node->next = curr;
        list->head = new_node;
        pthread_mutex_unlock(&list->head_lock);

        pthread_mutex_unlock(&curr->lock);
        return 0;
    }

    node_t *prev = NULL;

    // Traverse with lock coupling:
    // lock next, then unlock prev
    while (curr->next && curr->next->key < key) {
        node_t *next = curr->next;
        pthread_mutex_lock(&next->lock);

        if (prev)
            pthread_mutex_unlock(&prev->lock);

        prev = curr;
        curr = next;
    }

    // Insert after curr
    new_node->next = curr->next;
    curr->next = new_node;

    // Unlock held locks
    pthread_mutex_unlock(&curr->lock);
    if (prev)
        pthread_mutex_unlock(&prev->lock);

    return 0;
}

//Delete - Hand-over-hand locking
int list_delete(list_t *list, int key) {

    pthread_mutex_lock(&list->head_lock);

    node_t *curr = list->head;
    if (!curr) {
        pthread_mutex_unlock(&list->head_lock); 
        return -1;
    }

    pthread_mutex_lock(&curr->lock);
    pthread_mutex_unlock(&list->head_lock);

    node_t *prev = NULL;

    while (curr) {
        if (curr->key == key) {
            // Unlink node i.e.delete it from the list
            if (!prev) {
                pthread_mutex_lock(&list->head_lock);
                list->head = curr->next;
                pthread_mutex_unlock(&list->head_lock);
            } else {
                prev->next = curr->next;
            }

            // Unlock before destroying/freeing
            pthread_mutex_unlock(&curr->lock);
            if (prev)
                pthread_mutex_unlock(&prev->lock);

            pthread_mutex_destroy(&curr->lock);
            free(curr);
            return 0;
        }

        node_t *next = curr->next;
        if (!next) break;

        pthread_mutex_lock(&next->lock);

        if (prev)
            pthread_mutex_unlock(&prev->lock);

        prev = curr;
        curr = next;
    }

    // Not found: unlock what we hold
    pthread_mutex_unlock(&curr->lock);
    if (prev)
        pthread_mutex_unlock(&prev->lock);

    return -1;
}

//Lookup - Hand-over-hand locking
int list_lookup(list_t *list, int key) {

    pthread_mutex_lock(&list->head_lock);

    node_t *curr = list->head;
    if (!curr) { 
        pthread_mutex_unlock(&list->head_lock);
        return -1;
    } 

    pthread_mutex_lock(&curr->lock);
    pthread_mutex_unlock(&list->head_lock);

    while (curr) {
        if (curr->key == key) {
            pthread_mutex_unlock(&curr->lock);
            return 0;
        }

        node_t *next = curr->next;
        if (!next) break;

        pthread_mutex_lock(&next->lock);
        pthread_mutex_unlock(&curr->lock);

        curr = next;
    }

    pthread_mutex_unlock(&curr->lock);
    return -1;
}
