#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) { //! Extension : Ajout de argc/argv pour lire les arguments
    int sock;
    struct sockaddr_in broadcast_addr, from_addr;
    socklen_t addr_len = sizeof(from_addr);
    char buffer[1024];
    int broadcast_permission = 1;

    //! Extension : Valeur par défaut pour n (1 saut)
    char *hops_to_send = "1";

    //! Extension : Analyse des arguments (-hop n)
    if (argc == 3 && strcmp(argv[1], "-hop") == 0) {
        hops_to_send = argv[2];
    } else if (argc > 1) {
        //! Extension : Message d'aide si les arguments sont mal saisis
        printf("Usage: %s [-hop n]\n", argv[0]);
        return 1;
    }

    struct timeval tv;
    tv.tv_sec = 2;   // Temps d'attente : 2 secondes pleines
    tv.tv_usec = 0;  // 0 microseconde supplémentaire (1ms = 1000us)

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_permission, sizeof(broadcast_permission));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    broadcast_addr.sin_port = htons(9001);

    //! Extension : Affichage dynamique du nombre de sauts recherchés
    printf("Recherche des voisins (%s saut(s)) en cours...\n", hops_to_send);

    //! Extension : Envoi du nombre de sauts (hops_to_send) au lieu de "DISCOVER"
    sendto(sock, hops_to_send, strlen(hops_to_send), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    while (1) {
        int n = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&from_addr, &addr_len);

        if (n < 0) {
            break;
        }

        buffer[n] = '\0';
        printf("[Voisin trouvé] IP: %s | Nom: %s\n", inet_ntoa(from_addr.sin_addr), buffer);
    }

    printf("Fin de la recherche.\n");
    close(sock);
    return 0;
}