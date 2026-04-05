#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include "common.h"

#define MAX_CLIENTS 10

typedef struct {
    int sock;
    float cpu;
} Worker;

Worker workers[MAX_CLIENTS];
int worker_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int get_best_worker() {
    int idx = -1;
    float min = 101;
    for (int i = 0; i < worker_count; i++) {
        if (workers[i].cpu < min) {
            min = workers[i].cpu;
            idx = i;
        }
    }
    return idx;
}

void *handle_client(void *arg) {
    int sock = *(int*)arg;
    free(arg);

    while (1) {
        Header h;
        if (recv_all(sock, &h, sizeof(h)) < 0) break;

        char *buf = malloc(h.length);
        recv_all(sock, buf, h.length);

        if (h.type == MSG_CPU) {
            float cpu;
            memcpy(&cpu, buf, sizeof(float));
            pthread_mutex_lock(&lock);
            for (int i = 0; i < worker_count; i++) {
                if (workers[i].sock == sock) workers[i].cpu = cpu;
            }
            pthread_mutex_unlock(&lock);
        }
        else if (h.type == MSG_JOB) {
            pthread_mutex_lock(&lock);
            
            // Display pool status for PPT snapshots
            printf("\n--- Incoming Job: Worker Pool Status ---\n");
            for (int i = 0; i < worker_count; i++) {
                printf("Worker %d | Current Load: %.2f%%\n", i, workers[i].cpu);
            }

            int idx = get_best_worker();
            if (idx == -1) {
                printf("[Server] No workers available!\n");
                pthread_mutex_unlock(&lock);
                free(buf); continue;
            }

            printf("[Server] Selected Worker %d (Load: %.2f%%)\n", idx, workers[idx].cpu);
            int wsock = workers[idx].sock;
            pthread_mutex_unlock(&lock);

            send_msg(wsock, MSG_JOB, buf, h.length);
            Header rh;
            recv_all(wsock, &rh, sizeof(rh));
            char result[MAX_BUF] = {0};
            recv_all(wsock, result, rh.length);
            send_msg(sock, MSG_RESULT, result, rh.length);
        }
        free(buf);
    }
    close(sock);
    return NULL;
}

int main() {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr.s_addr = INADDR_ANY};
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 5);
    printf("[Server] Distributed System running on port %d\n", PORT);

    while (1) {
        int client = accept(server, NULL, NULL);
        pthread_mutex_lock(&lock);
        workers[worker_count].sock = client;
        workers[worker_count].cpu = 100.0; // Default high load until first update
        worker_count++;
        pthread_mutex_unlock(&lock);
        pthread_t t;
        int *p = malloc(sizeof(int)); *p = client;
        pthread_create(&t, NULL, handle_client, p);
    }
}
