#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#define PORT 8080
#define MAX_BUFFER 256
#define MAX_CONNECTIONS 5

void error(const char *msg) {
    perror(msg);
    WSACleanup();
    exit(1);
}

DWORD WINAPI handle_client(LPVOID client_socket_ptr) {
    SOCKET client_fd = *(SOCKET *)client_socket_ptr;
    free(client_socket_ptr);  // Libère la mémoire allouée dynamiquement
    char buffer[MAX_BUFFER];
    int i;

    for (i = 0; i < 60; i++) {
        time_t rawtime;
        time(&rawtime);
        snprintf(buffer, MAX_BUFFER, "%s", ctime(&rawtime));

        if (send(client_fd, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Échec de l'envoi à un client\n");
            break;
        }

        printf("Envoyé à un client : %s", buffer);
        Sleep(1000);
    }

    closesocket(client_fd);
    printf("Connexion client fermée.\n");
    return 0;
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, *client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup échoué: %d\n", WSAGetLastError());
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        error("Échec de la création du socket");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Échec du bind");
    }

    if (listen(server_fd, MAX_CONNECTIONS) == SOCKET_ERROR) {
        error("Échec du listen");
    }

    printf("Serveur TCP en écoute sur le port %d...\n", PORT);

    while (1) {
        client_fd = malloc(sizeof(SOCKET));
        if (!client_fd) {
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            continue;
        }

        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (*client_fd == INVALID_SOCKET) {
            free(client_fd);
            printf("Échec de l'acceptation de la connexion\n");
            continue;
        }

        printf("Nouveau client connecté\n");

        // Créer un thread pour traiter ce client
        HANDLE thread = CreateThread(NULL, 0, handle_client, client_fd, 0, NULL);
        if (thread == NULL) {
            printf("Échec de la création du thread\n");
            closesocket(*client_fd);
            free(client_fd);
        } else {
            CloseHandle(thread);  // Le thread se ferme de lui-même
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
