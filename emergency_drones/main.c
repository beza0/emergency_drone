#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "drone.h"
#include "survivor.h"
#include "ai.h"
#include "view.h"
#include <SDL2/SDL.h>  
#define DRONE_COUNT 10
int drone_count = DRONE_COUNT;

List survivor_list;

int main() {
    pthread_t drone_threads[DRONE_COUNT];
    pthread_t survivor_thread;
    pthread_t ai_thread;

    // Survivor listesi başlat
    list_init(&survivor_list);

    // Dron dizisi
    Drone drones[DRONE_COUNT];

    // Survivor üretici thread başlat
    pthread_create(&survivor_thread, NULL, survivor_generator, NULL);

    // AI thread başlat
    pthread_create(&ai_thread, NULL, mission_assigner, drones);

    // Dron thread'leri başlat
    for (int i = 0; i < DRONE_COUNT; i++) {
        init_drone(&drones[i], i, rand() % 40, rand() % 30);
        pthread_create(&drone_threads[i], NULL, drone_behavior, &drones[i]);
    }

    // ✅ SDL Başlat ve haritayı sürekli güncelle
    init_sdl();
   SDL_Event e;
int quit = 0;

while (!quit) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = 1;
        }
    }

    render_scene(drones, DRONE_COUNT);
}


    // (Bu kodlara normalde ulaşılmaz ama temizlik için bırakıyoruz)
    for (int i = 0; i < DRONE_COUNT; i++) {
        pthread_join(drone_threads[i], NULL);
    }

    pthread_join(survivor_thread, NULL);
    pthread_join(ai_thread, NULL);
    list_destroy(&survivor_list);

    return 0;
}
