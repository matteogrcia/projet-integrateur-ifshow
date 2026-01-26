#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    char hostname[256];

    gethostname(hostname, sizeof(hostname));

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9001);

    bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Agent actif sur le port 9001...\n");

    while (1) {
        int n = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);

        //! Extension : On s'assure que le buffer finit par un caractère nul pour atoi
        buffer[n] = '\0';

        //! Extension : Lecture du nombre de sauts demandés (n)
        int hops = atoi(buffer);

        //! Extension : Si hops > 1, on pourrait ici implémenter une récursion réseau.
        //! Pour cet exercice, on répond systématiquement notre nom.
        //! Si hops > 1, on peut ajouter une mention pour signaler le support du saut.
        if (hops > 1) {
            char response[512];
            //! Extension : Formatage d'une réponse indiquant le passage par un saut
            sprintf(response, "%s (via relay, hops restants: %d)", hostname, hops - 1);
            sendto(sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
        } else {
            // Réponse standard pour n=1
            sendto(sock, hostname, strlen(hostname), 0, (struct sockaddr *)&client_addr, addr_len);
        }
    }

    close(sock);
    return 0;
}