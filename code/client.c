#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    };

    inet_pton(AF_INET, argv[1], &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("[Client] Compiling test.c locally...\n");
    system("gcc test.c -o test_bin");

    FILE *f = fopen("test_bin", "rb");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    rewind(f);

    char *buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);

    printf("[Client] Sending job to server...\n");
    send_msg(sock, MSG_JOB, buf, size);

    Header h;
    recv_all(sock, &h, sizeof(h));

    char result[MAX_BUF] = {0};
    recv_all(sock, result, h.length);

    printf("[Client] Received %d bytes\n", h.length);

    printf("\n===== RESULT FROM REMOTE WORKER =====\n%s\n", result);

    free(buf);
    close(sock);
}
