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

    printf("[Worker] Connected to server\n");

    while (1) {
        // Send CPU usage
        float cpu = get_cpu();
        send_msg(sock, MSG_CPU, &cpu, sizeof(cpu));

        sleep(1);

        Header h;
        if (recv_all(sock, &h, sizeof(h)) < 0) break;

        if (h.type == MSG_JOB) {
            printf("[Worker] Job received\n");

            char *bin = malloc(h.length);
            recv_all(sock, bin, h.length);

            FILE *f = fopen("remote_job", "wb");
            fwrite(bin, 1, h.length, f);
            fclose(f);
            free(bin);

            system("chmod +x remote_job");

            FILE *fp = popen("./remote_job", "r");
            char output[MAX_BUF] = {0};
            int total = 0;

            while (fgets(output + total, sizeof(output) - total, fp) != NULL) {
                total = strlen(output);
            }

            pclose(fp);

            printf("[Worker] Sending result\n");

            send_msg(sock, MSG_RESULT, output, strlen(output));
        }
    }

    close(sock);
}
