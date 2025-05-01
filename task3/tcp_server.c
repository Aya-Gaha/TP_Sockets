#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#define PORT 8080
#define MAX_BUFFER 256
#define MAX_CONNECTIONS 5

void error(const char *msg) {
    perror(msg);
    WSACleanup();
    exit(1);
}

int main() {
    WSADATA wsaData;
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[MAX_BUFFER];
    int i;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Bind failed");
    }

    // Listen for connections
    if (listen(server_fd, MAX_CONNECTIONS) == SOCKET_ERROR) {
        error("Listen failed");
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            error("Accept failed");
        }

        printf("New client connected\n");

        // Send current time 60 times
        for (i = 0; i < 60; i++) {
            time_t rawtime;
            time(&rawtime);
            snprintf(buffer, MAX_BUFFER, "%s", ctime(&rawtime));

            // Send time to client
            if (send(client_fd, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
                error("Send failed");
            }

            printf("Sent: %s", buffer);
            Sleep(1000); // Introduce delay of 1 second (Sleep is in milliseconds on Windows)
        }

        // Close client connection
        printf("Finished sending to client\n");
        closesocket(client_fd);
    }

    // Close server socket and cleanup
    closesocket(server_fd);
    WSACleanup();
    return 0;
}