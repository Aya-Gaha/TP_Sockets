#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_BUFFER 256

void error(const char *msg) {
    perror(msg);
    WSACleanup();
    exit(1);
}

int main() {
    WSADATA wsaData;
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    int i;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_addr("127.0.0.1") == INADDR_NONE) {
        error("Invalid address");
    }
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Connection failed");
    }

    printf("Connected to server\n");

    // Receive 60 time messages
    for (i = 0; i < 60; i++) {
        int n = recv(sock_fd, buffer, MAX_BUFFER - 1, 0);
        if (n == SOCKET_ERROR) {
            error("Receive failed");
        }
        buffer[n] = '\0'; // Null-terminate the received string
        printf("Received: %s", buffer);
    }

    // Close socket and cleanup
    printf("Finished receiving messages\n");
    closesocket(sock_fd);
    WSACleanup();
    return 0;
}