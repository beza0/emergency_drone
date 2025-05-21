// drone.h
#ifndef DRONE_H
#define DRONE_H

#include <pthread.h>

typedef enum { IDLE, ON_MISSION } DroneStatus;

typedef struct {
    int x, y;
} Coord;

typedef struct {
    int id;
    DroneStatus status;
    Coord coord;
    Coord target;
    pthread_mutex_t lock;
} Drone;

void init_drone(Drone* d, int id, int x, int y);
void* drone_behavior(void* arg);

#endif
