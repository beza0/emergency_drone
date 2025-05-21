#include <stdlib.h>
#include "list.h"

void list_init(List* list) {
    list->head = NULL;
    pthread_mutex_init(&list->lock, NULL);
}

void list_add(List* list, void* data) {
    pthread_mutex_lock(&list->lock);
    Node* node = malloc(sizeof(Node));
    node->data = data;
    node->next = list->head;
    list->head = node;
    pthread_mutex_unlock(&list->lock);
}

void* list_pop(List* list) {
    pthread_mutex_lock(&list->lock);
    if (list->head == NULL) {
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }
    Node* node = list->head;
    list->head = node->next;
    void* data = node->data;
    free(node);
    pthread_mutex_unlock(&list->lock);
    return data;
}

void list_destroy(List* list) {
    pthread_mutex_lock(&list->lock);
    Node* curr = list->head;
    while (curr != NULL) {
        Node* tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    list->head = NULL;
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
}
