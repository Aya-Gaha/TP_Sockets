#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <time.h>


#define PORT 8080
#define MAX_BUFFER 256
#define MAX_CONNECTIONS 5

void error(const char *msg) {
    perror(msg);
    WSACleanup();
    exit(1);
}

//service1 : initiale(envoyer l'heure toutes les secondes pendant 60 secondes)
void send_time(SOCKET client_fd) {
    char buffer[MAX_BUFFER];
    for (int i = 0; i < 60; i++) {
        time_t rawtime;
        time(&rawtime);
        snprintf(buffer, MAX_BUFFER, "%s", ctime(&rawtime));
        send(client_fd, buffer, strlen(buffer), 0);
        Sleep(1000);
    }
}

//service2 : exécuter une commande système (tasklist) et envoyer le résultat au client
void execute_command(SOCKET client_fd) {
    FILE *fp = _popen("tasklist", "r");
    char buffer[MAX_BUFFER];
    if (!fp) {
        send(client_fd, "Failed to run command\n", 25, 0);
        return;
    }
    while (fgets(buffer, MAX_BUFFER, fp)) {
        send(client_fd, buffer, strlen(buffer), 0);
    }
    _pclose(fp);
}

//service3 : envoyer le contenu d'un fichier (data.txt) au client
void send_file(SOCKET client_fd) {
    FILE *fp = fopen("data.txt", "r");
    char buffer[MAX_BUFFER];
    if (!fp) {
        send(client_fd, "File not found\n", 15, 0);
        return;
    }
    while (fgets(buffer, MAX_BUFFER, fp)) {
        send(client_fd, buffer, strlen(buffer), 0);
    }
    fclose(fp);
}

unsigned __stdcall client_handler(void *arg) {
    SOCKET client_fd = (SOCKET)arg;
    char buffer[2];
    int n = recv(client_fd, buffer, 1, 0);
    if (n <= 0) {
        closesocket(client_fd);
        return 0;
    }
    buffer[n] = '\0';

    switch (buffer[0]) {
        case '1':
            send_time(client_fd);
            break;
        case '2':
            execute_command(client_fd);
            break;
        case '3':
            send_file(client_fd);
            break;
        default:
            send(client_fd, "Invalid service\n", 17, 0);
    }

    closesocket(client_fd);
    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        error("Socket creation failed");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Bind failed");
    }

    if (listen(server_fd, MAX_CONNECTIONS) == SOCKET_ERROR) {
        error("Listen failed");
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            error("Accept failed");
        }
        _beginthreadex(NULL, 0, client_handler, (void *)client_fd, 0, NULL);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
