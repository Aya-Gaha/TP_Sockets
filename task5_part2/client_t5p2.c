#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define MAX_BUFFER 1024

void error(const char *msg) {
    perror(msg);
    WSACleanup();
    exit(1);
}

int main() {
    WSADATA wsaData;
    SOCKET sock_fd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];
    int n, i;

    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup a échoué: %d\n", WSAGetLastError());
        return 1;
    }

    // Création du socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == INVALID_SOCKET) {
        error("Erreur de création du socket");
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connexion au serveur
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error("Échec de la connexion");
    }

    printf("Connecté au serveur.\n");
    printf("Entrez le numéro du service :\n");
    printf("1. Heure\n2. Commande système\n3. Transfert de fichier\n> ");
    fgets(buffer, MAX_BUFFER, stdin);

    // Envoi du choix au serveur
    if (send(sock_fd, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
        error("Échec de l'envoi du choix");
    }

    // Réception selon le service
    if (buffer[0] == '1') {
        // Réception de l'heure pendant 60 secondes
        for (i = 0; i < 60; i++) {
            n = recv(sock_fd, buffer, MAX_BUFFER - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            printf("Heure: %s", buffer);
        }
    } else {
        // Réception des données (commande ou fichier)
        while ((n = recv(sock_fd, buffer, MAX_BUFFER - 1, 0)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }
    }

    // Nettoyage
    closesocket(sock_fd);
    WSACleanup();
    printf("\nDéconnexion.\n");
    return 0;
}
