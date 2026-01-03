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

        //pointeur vers la valeur du masque de sous réseau sous forme d'unsigned char *  (car fait 1 octet)
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
        //pointeur vers la valeur du masque de sous réseau sous forme d'unsigned char *  (car fait 1 octet)
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


void to_string_ipv4(const char *ifname, struct sockaddr_in *addr, struct sockaddr *netmask)
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

    printf("%s %s/%d\n", ifname, ip_string, prefix);
}
void to_string_ipv6(const char *ifname, struct sockaddr_in6 *addr, struct sockaddr *netmask)
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
    printf("%s %s/%d\n", ifname, ip_string, prefix);
}

void show_all_interfaces(void)
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
                to_string_ipv4(ifa->ifa_name, (struct sockaddr_in *)ifa->ifa_addr, ifa->ifa_netmask);
            }
            else if (ifa->ifa_addr->sa_family == AF_INET6) {
                to_string_ipv6(ifa->ifa_name, (struct sockaddr_in6 *)ifa->ifa_addr, ifa->ifa_netmask);
            }
        }

        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifaddr);
}

void show_one_interface(const char *ifname)
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
                    to_string_ipv4(ifa->ifa_name, (struct sockaddr_in *)ifa->ifa_addr, ifa->ifa_netmask);
                }
                else if (ifa->ifa_addr->sa_family == AF_INET6) {
                    to_string_ipv6(ifa->ifa_name, (struct sockaddr_in6 *)ifa->ifa_addr, ifa->ifa_netmask);
                }
            }
        }

        ifa = ifa->ifa_next;
    }

    if (!found) {
        printf("Interface '%s' introuvable\n", ifname);
    }
    freeifaddrs(ifaddr);
}


int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "-a") == 0) {
        show_all_interfaces();
    }
    else if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        show_one_interface(argv[2]);
    }
    else {
        printf("Erreur lors de l'exécution");
        return 1;
    }

    return 0;
}
