#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "communication.h"

typedef struct {
    int x, y;
} Coord;

void send_json(int sock, const char* json) {
    write(sock, json, strlen(json));
}

void recv_json(int sock, char* buffer, size_t size) {
    int bytes = read(sock, buffer, size - 1);
    if (bytes > 0) buffer[bytes] = '\0';
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "KullanÄ±m: %s <drone_id>\n", argv[0]);
        return 1;
    }

    const char* drone_id = argv[1];
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("BaÄŸlantÄ± hatasÄ±");
        return 1;
    }

    Coord pos = {10, 10};
    Coord target = {10, 10};

    while (1) {
        // Durumu gÃ¶nder (idle)
        char status_msg[MAX_MSG];
        snprintf(status_msg, MAX_MSG,
            "{ \"type\": \"%s\", \"drone_id\": \"%s\", \"status\": \"idle\", \"location\": [%d, %d] }",
            MSG_STATUS_UPDATE, drone_id, pos.x, pos.y);
        send_json(sock, status_msg);

        // GÃ¶rev al
        char buffer[MAX_MSG];
        recv_json(sock, buffer, sizeof(buffer));
        printf("Gelen gÃ¶rev: %s\n", buffer);

        int tx, ty;
        if (sscanf(buffer, "{ \"type\": \"ASSIGN_MISSION\", \"target\": [%d, %d] }", &tx, &ty) == 2) {
            target.x = tx;
            target.y = ty;

            // GÃ¶revdeyken: hedefe ilerle
            while (pos.x != target.x || pos.y != target.y) {
                if (pos.x < target.x) pos.x++;
                else if (pos.x > target.x) pos.x--;

                if (pos.y < target.y) pos.y++;
                else if (pos.y > target.y) pos.y--;

                printf("â†’ (%d, %d)\n", pos.x, pos.y);

                snprintf(status_msg, MAX_MSG,
                    "{ \"type\": \"%s\", \"drone_id\": \"%s\", \"status\": \"on_mission\", \"location\": [%d, %d] }",
                    MSG_STATUS_UPDATE, drone_id, pos.x, pos.y);
                send_json(sock, status_msg);
                sleep(1);
            }

            // GÃ¶rev tamamlandÄ±
            snprintf(status_msg, MAX_MSG,
                "{ \"type\": \"%s\", \"drone_id\": \"%s\", \"location\": [%d, %d] }",
                MSG_MISSION_COMPLETE, drone_id, pos.x, pos.y);
            send_json(sock, status_msg);
            printf("âœ” GÃ¶rev tamamlandÄ±!\n");
        }

        sleep(2);
    }

    close(sock);
    return 0;
}
