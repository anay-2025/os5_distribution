#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"

float get_cpu() { 
    return (float)(rand() % 100); 
}

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
        perror("Connect failed");
        return 1;
    }

    printf("[Worker] Registered with Server at %s\n", argv[1]);
    fflush(stdout); // Force text to show in terminal

    while (1) {
        // Send CPU update
        float cpu = get_cpu();
        send_msg(sock, MSG_CPU, &cpu, sizeof(cpu));

        fd_set set;
        struct timeval tv = {1, 0}; // 1 second timeout
        FD_ZERO(&set);
        FD_SET(sock, &set);

        int rv = select(sock + 1, &set, NULL, NULL, &tv);

        if (rv > 0) {
            Header h;
            if (recv_all(sock, &h, sizeof(h)) < 0) break;

            if (h.type == MSG_JOB) {
                printf("\n[Worker] *** JOB RECEIVED FROM SERVER ***\n");
                printf("[Worker] Binary size: %d bytes\n", h.length);
                fflush(stdout);

                char *bin = malloc(h.length);
                recv_all(sock, bin, h.length);

                // Save and execute
                FILE *f = fopen("remote_job", "wb");
                fwrite(bin, 1, h.length, f);
                fclose(f);
                free(bin);

                system("chmod +x remote_job");

                printf("[Worker] Running binary...\n");
                fflush(stdout);

                FILE *fp = popen("./remote_job", "r");
                char output[MAX_BUF] = {0};
                fread(output, 1, sizeof(output), fp);
                pclose(fp);

                printf("[Worker] Execution finished. Sending result.\n");
                fflush(stdout);

                send_msg(sock, MSG_RESULT, output, strlen(output));
            }
        }
    }

    close(sock);
    return 0;
}
