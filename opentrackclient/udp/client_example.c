#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "headpose_lib.h"

int main(int argc, char *argv[]) {
    // 1. Check if both arguments are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip_address> <port>\n", argv[0]);
        fprintf(stderr, "Example: %s 127.0.0.1 5005\n", argv[0]);
        fprintf(stderr, "Use 0.0.0.0 to listen on all interfaces.\n");
        return 1;
    }

    const char* ip_address = argv[1];
    int port = atoi(argv[2]);

    // Simple port validation
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number %d. Must be between 1 and 65535.\n", port);
        return 1;
    }

    // 2. Initialize the library with the parsed IP and Port
    if (headpose_init(ip_address, port) != 0) {
        fprintf(stderr, "Failed to initialize headpose library on %s:%d\n", ip_address, port);
        return 1;
    }
    printf("Library initialized. Listening on UDP %s:%d...\n", ip_address, port);

    double current_pose[HEADPOSE_SIZE];

    // Read the loop data
    for (int i = 0; i < 10; i++) {
        headpose_get(current_pose);

        printf("Frame %d -> Pose: [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f]\n",
               i, current_pose[0], current_pose[1], current_pose[2],
                  current_pose[3], current_pose[4], current_pose[5]);

        usleep(500000);
    }

    headpose_close();
    printf("Library shut down gracefully.\n");
    return 0;
}
