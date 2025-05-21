#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "list.h"
#include "survivor.h"

extern List survivor_list;

void* survivor_generator(void* arg) {
    int counter = 0;
    srand(time(NULL));  // Rastgelelik için

    while (1) {
        Survivor* s = malloc(sizeof(Survivor));
        s->id = counter++;
        s->x = rand() % 40;  // 40x30 harita varsayalım
        s->y = rand() % 30;
        s->assigned = 0;     // ✅ Başlangıçta görev atanmamış

        list_add(&survivor_list, s);

        printf("Survivor %d created at (%d, %d)\n", s->id, s->x, s->y);
        sleep(4);  // 4 saniyede bir survivor oluştur
    }

    return NULL;
}
