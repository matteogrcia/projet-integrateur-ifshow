#include <stdio.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <netinet/in.h>


int get_prefix_length(struct sockaddr *netmask)
{
    int count = 0;
    unsigned char *bytes;
    int i;

    if (netmask->sa_family == AF_INET) {
        bytes = (unsigned char *)
                &((struct sockaddr_in *)netmask)->sin_addr;

        for (i = 0; i < 4; i++) {
            unsigned char b = bytes[i];
            while (b) {
                count += b & 1;
                b >>= 1;
            }
        }
    }
    else if (netmask->sa_family == AF_INET6) {
        bytes = (unsigned char *)
                &((struct sockaddr_in6 *)netmask)->sin6_addr;

        for (i = 0; i < 16; i++) {
            unsigned char b = bytes[i];
            while (b) {
                count += b & 1;
                b >>= 1;
            }
        }
    }

    return count;
}

/* MODIFICATION : Ajout de numSortie pour rediriger l'affichage vers l'écran (1) ou le réseau */
void to_string_ipv4(const char *ifname, struct sockaddr_in *addr, struct sockaddr *netmask, int numSortie)
{
    char ip_string[INET_ADDRSTRLEN];

    inet_ntop(
        AF_INET,
        &(addr->sin_addr),
        ip_string,
        INET_ADDRSTRLEN
    );
    int prefix = 0;
    if (netmask != NULL)
        prefix = get_prefix_length(netmask);

    /* Remplacement de printf par dprintf vers numSortie */
    dprintf(numSortie, "%s %s/%d\n", ifname, ip_string, prefix);
}

/* MODIFICATION : Ajout de numSortie */
void to_string_ipv6(const char *ifname, struct sockaddr_in6 *addr, struct sockaddr *netmask, int numSortie)
{
    char ip_string[INET6_ADDRSTRLEN];

    inet_ntop(
        AF_INET6,
        &(addr->sin6_addr),
        ip_string,
        INET6_ADDRSTRLEN
    );
    int prefix = 0;
    if (netmask != NULL)
        prefix = get_prefix_length(netmask);

    /* Remplacement de printf par dprintf vers numSortie */
    dprintf(numSortie, "%s %s/%d\n", ifname, ip_string, prefix);
}

/* MODIFICATION : La fonction reçoit numSortie et le transmet aux fonctions to_string */
void show_all_interfaces(int numSortie)
{
    struct ifaddrs *ifaddr;
    struct ifaddrs *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        return;
    }

    ifa = ifaddr;
    while (ifa != NULL) {

        if (ifa->ifa_addr != NULL) {

            if (ifa->ifa_addr->sa_family == AF_INET) {
                to_string_ipv4(ifa->ifa_name, (struct sockaddr_in *)ifa->ifa_addr, ifa->ifa_netmask, numSortie);
            }
            else if (ifa->ifa_addr->sa_family == AF_INET6) {
                to_string_ipv6(ifa->ifa_name, (struct sockaddr_in6 *)ifa->ifa_addr, ifa->ifa_netmask, numSortie);
            }
        }

        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifaddr);
}

/* MODIFICATION : La fonction reçoit numSortie */
void show_one_interface(const char *ifname, int numSortie)
{
    struct ifaddrs *ifaddr;
    struct ifaddrs *ifa;
    int found = 0;

    if (getifaddrs(&ifaddr) == -1) {
        return;
    }

    ifa = ifaddr;
    while (ifa != NULL) {

        if (ifa->ifa_addr != NULL) {

            if (strcmp(ifa->ifa_name, ifname) == 0) {
                found=1;
                if (ifa->ifa_addr->sa_family == AF_INET) {
                    to_string_ipv4(ifa->ifa_name, (struct sockaddr_in *)ifa->ifa_addr, ifa->ifa_netmask, numSortie);
                }
                else if (ifa->ifa_addr->sa_family == AF_INET6) {
                    to_string_ipv6(ifa->ifa_name, (struct sockaddr_in6 *)ifa->ifa_addr, ifa->ifa_netmask, numSortie);
                }
            }
        }

        ifa = ifa->ifa_next;
    }

    if (!found) {
        /* Envoi du message d'erreur vers la sortie choisie */
        dprintf(numSortie, "Interface '%s' introuvable\n", ifname);
    }
    freeifaddrs(ifaddr);
}


/* MODIFICATION : On protège le main pour qu'il ne soit compilé que pour l'outil local (gcc -DEXECUTABLE_IFSHOW ifshow.c -o ifshow) */
#ifdef EXECUTABLE_IFSHOW
int main(int argc, char *argv[])
{
    /* 1 représente la sortie standard */
    if (argc == 2 && strcmp(argv[1], "-a") == 0) {
        show_all_interfaces(1);
    }
    else if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        show_one_interface(argv[2], 1);
    }
    else {
        fprintf(stderr, "Erreur lors de l'exécution\n");
        return 1;
    }

    return 0;
}
#endif