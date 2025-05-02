#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

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
    int server_len = sizeof(server_addr);
    char buffer[MAX_BUFFER];
    int i,n;

    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup echoue: %d\n", WSAGetLastError());
        return 1;
    }

    // Création du socket UDP
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == INVALID_SOCKET) {
        error("Echec de la creation du socket");
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 1. Envoi de "Bonjour"
    sendto(sock_fd, "Bonjour", strlen("Bonjour"), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Envoye: Bonjour\n");


    // Envoi de 60 messages d'heure
    for (i = 0; i < 60; i++) {
        int n = recvfrom(sock_fd, buffer, MAX_BUFFER - 1, 0, NULL, NULL);
        if (n == SOCKET_ERROR) {
            error("Échec de la reception");
        }
        buffer[n] = '\0'; // Terminer la chaîne reçue
        printf("Recu: %s", buffer);
    }

    // 3. Envoi de "Au revoir"
    sendto(sock_fd, "Au revoir", strlen("Au revoir"), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Envoye: Au revoir\n");

    // Fermeture du socket et nettoyage
    printf("Fin de la reception des messages\n");
    closesocket(sock_fd);
    WSACleanup();
    return 0;
}
