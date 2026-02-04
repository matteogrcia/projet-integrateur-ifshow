#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h> // Nécessaire pour lister les interfaces
#include <net/if.h>
int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    char buffer[1024];
    int broadcast_permission = 1;
    char *hops_to_send = (argc == 3 && strcmp(argv[1], "-hop") == 0) ? argv[2] : "1";

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_permission, sizeof(broadcast_permission));

    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // --- PARTIE CRUCIALE : Itération sur les interfaces ---
    struct ifaddrs *ifap, *ifa;
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        return 1;
    }

    printf("Recherche des voisins sur toutes les interfaces...\n");

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        // On ne s'intéresse qu'aux interfaces IPv4 (AF_INET) qui sont actives et gèrent le broadcast
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET && (ifa->ifa_flags & IFF_BROADCAST)) {

            struct sockaddr_in *bcast = (struct sockaddr_in *)ifa->ifa_broadaddr;
            bcast->sin_port = htons(9001);

            printf(" > Envoi sur %s (Broadcast: %s)\n", ifa->ifa_name, inet_ntoa(bcast->sin_addr));

            sendto(sock, hops_to_send, strlen(hops_to_send), 0, (struct sockaddr *)bcast, sizeof(struct sockaddr_in));
        }
    }
    freeifaddrs(ifap);
    // -------------------------------------------------------

    while (1) {
        int n = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&from_addr, &addr_len);
        if (n < 0) break;
        buffer[n] = '\0';
        printf("[Voisin trouvé] IP: %s | Nom: %s\n", inet_ntoa(from_addr.sin_addr), buffer);
    }

    printf("Fin de la recherche.\n");
    close(sock);
    return 0;
}