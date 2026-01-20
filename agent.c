#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Inclusion de ton fichier d'en-tête */
#include "ifshow.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int socket_serveur, socket_client;
    struct sockaddr_in adresse_serveur;
    char buffer[BUFFER_SIZE];

    // 1. Création de la socket
    socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_serveur == -1) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    // 2. Configuration de l'adresse
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_addr.s_addr = INADDR_ANY;
    adresse_serveur.sin_port = htons(PORT);

    // 3. Bind (Lier la socket au port)
    if (bind(socket_serveur, (struct sockaddr *)&adresse_serveur, sizeof(adresse_serveur)) < 0) {
        perror("Erreur Bind");
        exit(EXIT_FAILURE);
    }

    // 4. Listen (Attente de connexions)
    listen(socket_serveur, 5);
    printf("Agent prêt ! Lancement manuellement avec ./agent\n");
    printf("En attente de commandes sur le port %d...\n", PORT);

    while (1) {
        // 5. Accept (Attend qu'un client se connecte)
        socket_client = accept(socket_serveur, NULL, NULL);
        if (socket_client < 0) {
            perror("Erreur Accept");
            continue;
        }

        // 6. Réception de la commande
        memset(buffer, 0, BUFFER_SIZE);// "Nettoie" la mémoire en remplissant le tableau de zéros pour éviter les conflits entre deux messages
        read(socket_client, buffer, BUFFER_SIZE - 1);

        // 7. Analyse et exécution via ifshow.c
        if (strncmp(buffer, "-a", 2) == 0) {
            show_all_interfaces(socket_client);
        }
        else if (strncmp(buffer, "-i ", 3) == 0) {
            char *nom = buffer + 3;
            nom[strcspn(nom, "\n\r")] = 0; // Nettoyage
            show_one_interface(nom, socket_client);
        }

        // 8. Fermeture de la session client
        close(socket_client);
    }

    return 0;
}