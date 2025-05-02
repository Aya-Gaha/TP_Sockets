#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <windows.h> // pour Sleep


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
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[MAX_BUFFER];
    int i;

    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup echoue: %d\n", WSAGetLastError());
        return 1;
    }

    // Création du socket UDP
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == INVALID_SOCKET) {
        error("Echec de la creation du socket");
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Lier le socket à une adresse
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Echec du bind");
    }

    printf("Serveur UDP à l'ecoute sur le port %d...\n", PORT);


    // 1. Attendre "Bonjour"
    int n = recvfrom(server_fd, buffer, MAX_BUFFER - 1, 0, (struct sockaddr*)&client_addr, &client_len);
    if (n == SOCKET_ERROR) {
        error("Echec de la reception");
    }
    buffer[n] = '\0';
    printf("Recu: %s\n", buffer);

    if (strcmp(buffer, "Bonjour") != 0) {
        printf("Message inattendu. Fermeture.\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Envoi de l'heure à chaque client
    for (i = 0; i < 60; i++) {
        time_t rawtime;
        time(&rawtime);
        snprintf(buffer, MAX_BUFFER, "%s", ctime(&rawtime));

        // Envoi de l'heure à un client
        if (sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len) == SOCKET_ERROR) {
            error("Echec de l'envoi");
        }

        printf("Envoye: %s", buffer);
        Sleep(1000); // Délai d'une seconde
    }


    // 3. Attendre "Au revoir"
    n = recvfrom(server_fd, buffer, MAX_BUFFER - 1, 0, (struct sockaddr*)&client_addr, &client_len);
    if (n == SOCKET_ERROR) {
        error("Echec de la reception finale");
    }
    buffer[n] = '\0';
    printf("Recu: %s\n", buffer);

    if (strcmp(buffer, "Au revoir") == 0) {
        printf("Communication terminee proprement.\n");
    } else {
        printf("Message final inattendu.\n");
    }

    // Fermeture du socket et nettoyage
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
