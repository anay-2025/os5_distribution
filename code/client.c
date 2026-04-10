#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"

float get_worker_score(char *ip, int port) {
    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return 9999;
    }

    send(sock, "LOAD", 4, 0);

    WorkerStats stats;
    recv(sock, &stats, sizeof(stats), 0);

    close(sock);

    float score = stats.cpu / stats.cores;

    printf("Worker %d -> CPU: %.2f%% | Cores: %d | Score: %.2f\n",
           port, stats.cpu, stats.cores, score);

    return score;
}

int connect_to_worker(char *ip, int port) {
    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }

    return sock;
}

int main() {

    if (system("gcc test.c -o test") != 0) {
        perror("Compilation failed");
        return 1;
    }

    char *workers[] = {"172.30.1.112", "172.30.5.60", "172.30.2.230"};
    int ports[] = {8080, 8081};

    float min_score = 9999;
    int best = -1;

    for (int i = 0; i < 2; i++) {
        float score = get_worker_score(workers[i], ports[i]);

        if (score < min_score) {
            min_score = score;
            best = i;
        }
    }

    if (best == -1) {
        printf("❌ No workers available\n");
        return 1;
    }

    printf("🔥 Selected worker at port %d\n", ports[best]);

    int sock = connect_to_worker(workers[best], ports[best]);

    FILE *fp = fopen("test", "rb");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    int bytes;

    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sock, buffer, bytes, 0);
    }

    fclose(fp);
    shutdown(sock, SHUT_WR);

    printf("\n===== OUTPUT =====\n");

    while ((bytes = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        ssize_t w = write(1, buffer, bytes);
        if (w < 0) perror("write failed");
    }

    printf("\n");
    close(sock);

    return 0;
}
