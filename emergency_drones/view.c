#include <SDL2/SDL.h>
#include <stdio.h>
#include "view.h"
#include "list.h"
#include "survivor.h"

#define WIDTH 800
#define HEIGHT 600
#define CELL_SIZE 20

extern List survivor_list;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void init_sdl() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Drone Simulation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void draw_cell(int x, int y, SDL_Color color) {
    SDL_Rect rect = {
        .x = x * CELL_SIZE,
        .y = y * CELL_SIZE,
        .w = CELL_SIZE,
        .h = CELL_SIZE
    };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void render_scene(Drone* drones, int drone_count) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 🔒 Listeyi hızlıca kopyala
    Survivor* temp_survivors[100];
    int survivor_count = 0;

    pthread_mutex_lock(&survivor_list.lock);
    Node* curr = survivor_list.head;
    while (curr != NULL && survivor_count < 100) {
        temp_survivors[survivor_count++] = (Survivor*)curr->data;
        curr = curr->next;
    }
    pthread_mutex_unlock(&survivor_list.lock);

    // 🔄 Şimdi lock yok, rahatça çizebiliriz
    for (int i = 0; i < survivor_count; i++) {
        draw_cell(temp_survivors[i]->x, temp_survivors[i]->y, (SDL_Color){255, 0, 0});
    }

    for (int i = 0; i < drone_count; i++) {
        pthread_mutex_lock(&drones[i].lock);
        SDL_Color color = drones[i].status == IDLE ? (SDL_Color){0, 0, 255} : (SDL_Color){0, 255, 0};
        draw_cell(drones[i].coord.x, drones[i].coord.y, color);
        pthread_mutex_unlock(&drones[i].lock);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(50);  // Daha akıcı ve az yük için 100 yerine 50ms
}

