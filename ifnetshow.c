#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char commande[128];

    // 1. Vérification des arguments (ex: ./ifnetshow 192.168.1.10 -a)
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <IP_AGENT> <OPTION [-a | -i name]>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 2. Création de la socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur création socket");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 3. Conversion de l'adresse IP (Texte vers Binaire)
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Adresse IP invalide ou non supportée\n");
        return -1;
    }

    // 4. Connexion à l'Agent
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("La connexion a échoué (L'agent est-il lancé ?)");
        return -1;
    }

    // 5. Préparation de la commande à envoyer
    // On concatène les arguments pour former "-a" ou "-i eth0"
    if (argc == 3) {
        snprintf(commande, sizeof(commande), "%s", argv[2]);
    } else {
        snprintf(commande, sizeof(commande), "%s %s", argv[2], argv[3]);
    }

    // 6. Envoi de la commande
    send(sock, commande, strlen(commande), 0);

    // 7. Lecture de la réponse de l'agent et affichage
    int octets_lus;
    while ((octets_lus = read(sock, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[octets_lus] = '\0'; // On s'assure que c'est une chaîne valide
        printf("%s", buffer);
    }

    // 8. Fermeture
    close(sock);
    return 0;
}