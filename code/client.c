#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

int main(int argc, char *argv[]) {
    // Check if server IP is provided
    if (argc < 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    };

    if (inet_pton(AF_INET, argv[1], &server.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Compile the local source file into a binary named 'test_bin'
    printf("[Client] Compiling test.c locally...\n");
    system("gcc test.c -o test_bin");

    FILE *f = fopen("test_bin", "rb");
    if (!f) {
        perror("Failed to open binary");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    rewind(f);

    char *buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);

    // Send the compiled binary to the server
    printf("[Client] Sending job to server...\n");
    send_msg(sock, MSG_JOB, buf, size);

    // Prepare to receive the execution result
    Header h;
    if (recv_all(sock, &h, sizeof(h)) < 0) {
        printf("Failed to receive header\n");
        free(buf);
        close(sock);
        return 1;
    }

    // Initialize buffer with zeros and ensure null-termination
    char result[MAX_BUF] = {0}; 
    recv_all(sock, result, h.length);
    result[h.length] = '\0'; // Explicitly terminate the string

    printf("\n===== RESULT FROM REMOTE WORKER =====\n%s\n", result);

    free(buf);
    close(sock);
    return 0;
}
