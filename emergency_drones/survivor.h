#ifndef SURVIVOR_H
#define SURVIVOR_H

typedef struct {
    int id;
    int x, y;
    int assigned;  // ✅ Yeni alan: görev verildi mi?
} Survivor;

void* survivor_generator(void* arg);

#endif
