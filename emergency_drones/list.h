#ifndef LIST_H
#define LIST_H

#include <pthread.h>

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
    pthread_mutex_t lock;
} List;

void list_init(List* list);
void list_add(List* list, void* data);
void* list_pop(List* list);
void list_destroy(List* list);

#endif
