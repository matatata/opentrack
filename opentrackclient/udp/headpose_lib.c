#include "headpose_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Added for inet_pton

static double internal_pose[HEADPOSE_SIZE] = {0.0};
static pthread_mutex_t pose_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_t listener_thread;
static int server_fd = -1;
static volatile int keep_running = 0;

static void* udp_listener_thread(void* arg) {
    double buffer[HEADPOSE_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (keep_running) {
        ssize_t bytes_received = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                                          (struct sockaddr*)&client_addr, &addr_len);

        if (bytes_received == sizeof(buffer)) {
            pthread_mutex_lock(&pose_mutex);
            memcpy(internal_pose, buffer, sizeof(internal_pose));
            pthread_mutex_unlock(&pose_mutex);
        } else if (bytes_received < 0) {
            if (!keep_running) break;
            perror("UDP read error");
        }
    }
    return NULL;
}

int headpose_init(const char* ip_address, int port) {
    struct sockaddr_in server_addr;

    // 1. Create UDP socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind to specific IP and Port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert IP text string to binary network format
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address format: %s\n", ip_address);
        close(server_fd);
        server_fd = -1;
        return -1;
    }

    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        server_fd = -1;
        return -1;
    }

    // 3. Start background thread
    keep_running = 1;
    if (pthread_create(&listener_thread, NULL, udp_listener_thread, NULL) != 0) {
        perror("Thread creation failed");
        close(server_fd);
        server_fd = -1;
        keep_running = 0;
        return -1;
    }

    return 0;
}

void headpose_get(double out_pose[HEADPOSE_SIZE]) {
    pthread_mutex_lock(&pose_mutex);
    memcpy(out_pose, internal_pose, sizeof(double) * HEADPOSE_SIZE);
    pthread_mutex_unlock(&pose_mutex);
}

void headpose_close(void) {
    if (!keep_running) return;

    keep_running = 0;
    if (server_fd >= 0) {
        close(server_fd);
    }

    pthread_join(listener_thread, NULL);
    pthread_mutex_destroy(&pose_mutex);
}
