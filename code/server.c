#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"

// 🔥 Get CPU usage
float get_cpu_load() {
    FILE *fp = popen("top -bn1 | grep \"Cpu(s)\" | awk '{print 100 - $8}'", "r");
    if (!fp) return -1;

    float load = -1;
    if (fscanf(fp, "%f", &load) != 1) {
        load = -1;
    }

    pclose(fp);
    return load;
}

// 🔥 Get number of cores
int get_cpu_cores() {
    FILE *fp = popen("nproc", "r");
    if (!fp) return 1;

    int cores = 1;
    if (fscanf(fp, "%d", &cores) != 1) {
        cores = 1;
    }

    pclose(fp);
    return cores;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int PORT = atoi(argv[1]);

    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("🚀 Worker running on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        char buffer[BUFFER_SIZE] = {0};
        int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes <= 0) {
            close(client_socket);
            continue;
        }

        // 🔥 LOAD request
        if (strncmp(buffer, "LOAD", 4) == 0) {
            WorkerStats stats;
            stats.cpu = get_cpu_load();
            stats.cores = get_cpu_cores();

            send(client_socket, &stats, sizeof(stats), 0);
            close(client_socket);
            continue;
        }

        // 🔥 Receive file
        FILE *fp = fopen("received_exec", "wb");
        if (!fp) {
            perror("File open failed");
            close(client_socket);
            continue;
        }

        fwrite(buffer, 1, bytes, fp);

        while ((bytes = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes, fp);
        }

        fclose(fp);

        // 🔥 Compile
        int compile_status = system("gcc received_exec -o exec_bin 2> compile_error.txt");

        if (compile_status != 0) {
            if (system("chmod +x received_exec") != 0) {
                perror("chmod failed");
            }
            if (system("./received_exec > output.txt 2>&1") != 0) {
                perror("execution failed");
            }
        } else {
            if (system("./exec_bin > output.txt 2>&1") != 0) {
                perror("execution failed");
            }
        }

        // 🔥 Send output
        fp = fopen("output.txt", "rb");
        if (!fp) {
            perror("Output open failed");
            close(client_socket);
            continue;
        }

        while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
            send(client_socket, buffer, bytes, 0);
        }

        fclose(fp);
        close(client_socket);
    }

    return 0;
}
