#ifndef HEADPOSE_LIB_H
#define HEADPOSE_LIB_H

#define HEADPOSE_SIZE 6

// Initializes the library, starts the UDP listener on a specific IP and port.
// Pass "0.0.0.0" to listen on all interfaces.
// Returns 0 on success, -1 on failure.
int headpose_init(const char* ip_address, int port);

// Thread-safely retrieves the latest headpose data.
void headpose_get(double out_pose[HEADPOSE_SIZE]);

// Cleans up resources and stops the background thread.
void headpose_close(void);

#endif // HEADPOSE_LIB_H
