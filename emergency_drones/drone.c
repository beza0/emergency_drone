#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "drone.h"
#include "list.h"
#include "survivor.h"

extern List survivor_list;

void init_drone(Drone* d, int id, int x, int y) {
    d->id = id;
    d->status = IDLE;
    d->coord.x = x;
    d->coord.y = y;
    d->target.x = x;
    d->target.y = y;
    pthread_mutex_init(&d->lock, NULL);
}

void* drone_behavior(void* arg) {
    Drone* d = (Drone*)arg;

    while (1) {
        pthread_mutex_lock(&d->lock);

        if (d->status == ON_MISSION) {
            if (d->coord.x < d->target.x) d->coord.x++;
            else if (d->coord.x > d->target.x) d->coord.x--;

            if (d->coord.y < d->target.y) d->coord.y++;
            else if (d->coord.y > d->target.y) d->coord.y--;

            printf("Drone %d moved to (%d, %d)\n", d->id, d->coord.x, d->coord.y);

            if (d->coord.x == d->target.x && d->coord.y == d->target.y) {
                // ✅ Güvenli survivor silme
                pthread_mutex_lock(&survivor_list.lock);
                Node* prev = NULL;
                Node* curr = survivor_list.head;

                while (curr != NULL) {
                    Survivor* s = (Survivor*)curr->data;
                    if (s->x == d->coord.x && s->y == d->coord.y) {
                        Node* to_delete = curr;

                        if (prev == NULL)
                            survivor_list.head = curr->next;
                        else
                            prev->next = curr->next;

                        curr = curr->next; // 🔐 Free etmeden önce ilerlet

                        free(to_delete->data);
                        free(to_delete);
                        break;
                    } else {
                        prev = curr;
                        curr = curr->next;
                    }
                }

                pthread_mutex_unlock(&survivor_list.lock);

                d->status = IDLE;
                printf("Drone %d: Mission completed!\n", d->id);
            }
        }

        pthread_mutex_unlock(&d->lock);
        sleep(1); // Her saniye bir hareket
    }

    return NULL;
}
