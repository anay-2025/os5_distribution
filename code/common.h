#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_BUF 65536

typedef enum {
    MSG_CPU = 1,
    MSG_JOB,
    MSG_RESULT
} MsgType;

typedef struct {
    int type;
    int length;
} Header;

int send_all(int sock, void *buf, int len) {
    int total = 0;
    while (total < len) {
        int n = send(sock, (char*)buf + total, len - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return 0;
}

int recv_all(int sock, void *buf, int len) {
    int total = 0;
    while (total < len) {
        int n = recv(sock, (char*)buf + total, len - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return 0;
}

int send_msg(int sock, int type, void *data, int len) {
    Header h = {type, len};
    if (send_all(sock, &h, sizeof(h)) < 0) return -1;
    if (len > 0 && send_all(sock, data, len) < 0) return -1;
    return 0;
}

#endif
