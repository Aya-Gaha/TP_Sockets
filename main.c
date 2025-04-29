#define _WIN32_WINNT 0x0601

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // important pour Windows
#include <ws2tcpip.h> // pour getaddrinfo
#pragma comment(lib, "ws2_32.lib") // linker automatiquement la librairie

#define PORT "80"
#define BUFFER_SIZE 4096

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct addrinfo hints, *res;
    char buffer[BUFFER_SIZE];
    char userRequest[BUFFER_SIZE];
    int bytesReceived;

    // 1. Initialiser Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Erreur WSAStartup : %d\n", WSAGetLastError());
        return 1;
    }

    // 2. Créer le socket
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    if (getaddrinfo("example.com", PORT, &hints, &res) != 0) {
        printf("Erreur getaddrinfo\n");
        WSACleanup();
        return 1;
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        printf("Erreur socket : %d\n", WSAGetLastError());
        freeaddrinfo(res);
        WSACleanup();
        return 1;
    }

    // 3. Connecter au serveur
    if (connect(sock, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        printf("Erreur connect : %d\n", WSAGetLastError());
        closesocket(sock);
        freeaddrinfo(res);
        WSACleanup();
        return 1;
    }

    printf("Connexion réussie à example.com sur le port 80.\n");

    freeaddrinfo(res); // libérer la mémoire de getaddrinfo

    // 4. Préparer ou lire la requête HTTP
    memset(userRequest, 0, BUFFER_SIZE);
    printf("Entrez votre requête HTTP (ou appuyez sur Entrée pour utiliser la requête par défaut) :\n");

    fgets(userRequest, BUFFER_SIZE, stdin);

    // Enlever le retour chariot '\n'
    size_t len = strlen(userRequest);
    if (len > 0 && userRequest[len - 1] == '\n') {
        userRequest[len - 1] = '\0';
    }

    const char *defaultRequest = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    const char *httpRequest;

    if (strlen(userRequest) == 0) {
        httpRequest = defaultRequest;
        printf("Utilisation de la requête par défaut.\n");
    } else {
        httpRequest = userRequest;
        printf("Utilisation de votre requête.\n");
    }

    // 5. Envoyer la requête au serveur
    if (send(sock, httpRequest, (int)strlen(httpRequest), 0) == SOCKET_ERROR) {
        printf("Erreur send : %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Requête envoyée ! Réception de la réponse...\n");

    // 6. Recevoir la réponse du serveur
    do {
        memset(buffer, 0, BUFFER_SIZE); // vider le buffer
        bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            printf("%s", buffer); // afficher la réponse reçue
        }
    } while (bytesReceived > 0);

    if (bytesReceived == 0) {
        printf("\nConnexion fermée par le serveur.\n");
    } else if (bytesReceived < 0) {
        printf("\nErreur de réception : %d\n", WSAGetLastError());
    }

    // 7. Nettoyer
    closesocket(sock);
    WSACleanup();
    return 0;
}
