#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "list.h"
#include "drone.h"
#include "survivor.h"

extern List survivor_list;
extern int drone_count;

int distance(Coord a, Coord b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

void* mission_assigner(void* arg) {
    Drone* drones = (Drone*)arg;
    while (1) {
        pthread_mutex_lock(&survivor_list.lock);

        Node* curr = survivor_list.head;
        while (curr != NULL) {
            Survivor* s = (Survivor*)curr->data;
if (s->assigned) {
    curr = curr->next;
    continue;  // Görev verilmişse atla
}


            // En yakın boşta drone'u bul
            int min_dist = 99999;
            int selected = -1;

            for (int i = 0; i < drone_count; i++) {

                pthread_mutex_lock(&drones[i].lock);
                if (drones[i].status == IDLE) {
                    int dist = distance(drones[i].coord, (Coord){s->x, s->y});
                    if (dist < min_dist) {
                        min_dist = dist;
                        selected = i;
                    }
                }
                pthread_mutex_unlock(&drones[i].lock);
            }

            // Eğer drone bulunduysa ata
            if (selected != -1) {
            s->assigned = 1; 
                pthread_mutex_lock(&drones[selected].lock);
                drones[selected].target.x = s->x;
                drones[selected].target.y = s->y;
                drones[selected].status = ON_MISSION;
                 
                pthread_mutex_unlock(&drones[selected].lock);

                printf("AI: Survivor %d at (%d,%d) → Drone %d atanıyor\n",
                    s->id, s->x, s->y, selected);

                // Survivor listeden çıkarılıyor
  //             Node* to_delete = curr;
  //             curr = curr->next;

                // list_pop değil çünkü ortadaki node'u siliyoruz
  //              Node* prev = survivor_list.head;
   //             if (prev == to_delete) {
   //                 survivor_list.head = curr;
   //             } else {
   //                 while (prev && prev->next != to_delete)
   //                     prev = prev->next;
   //                 if (prev) prev->next = to_delete->next;
  //              }

   //             free(to_delete->data);
  //              free(to_delete);
  //              continue;
   
            }
 curr = curr->next;
          
        }

        pthread_mutex_unlock(&survivor_list.lock);
        sleep(1);
    }
    return NULL;
}
