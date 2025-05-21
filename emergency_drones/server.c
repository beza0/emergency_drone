#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include "communication.h"

void* handle_drone(void* arg) {
    int drone_fd = *(int*)arg;
    char buffer[MAX_MSG];
if (strstr(buffer, "STATUS_UPDATE")) {
    char reply[MAX_MSG];
    snprintf(reply, MAX_MSG,
        "{ \"type\": \"%s\", \"target\": [%d, %d] }",
        MSG_ASSIGN_MISSION, rand() % 40, rand() % 30);
    write(drone_fd, reply, strlen(reply));
}
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(drone_fd, buffer, sizeof(buffer));
        if (bytes <= 0) {
            printf("Drone bağlantısı kesildi\n");
            break;
        }

        printf("Alındı: %s\n", buffer);

        // Basit cevap döneriz (örnek mission)
        char reply[MAX_MSG];
        snprintf(reply, MAX_MSG,
                 "{ \"type\": \"%s\", \"target\": [5, 10] }",
                 MSG_ASSIGN_MISSION);
        write(drone_fd, reply, strlen(reply));
    }

    close(drone_fd);
    return NULL;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(server_fd, 10);

    printf("Sunucu %d portunda dinleniyor...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int drone_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);

        pthread_t t;
        int* pclient = malloc(sizeof(int));
        *pclient = drone_fd;
        pthread_create(&t, NULL, handle_drone, pclient);
    }

    return 0;
}
