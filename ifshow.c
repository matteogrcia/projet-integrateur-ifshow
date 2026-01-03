#include <stdio.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <netinet/in.h>

void to_string_ipv4(struct sockaddr_in *addr)
{
    char ip_string[INET_ADDRSTRLEN];

    inet_ntop(
        AF_INET,
        &(addr->sin_addr),
        ip_string,
        INET_ADDRSTRLEN
    );

    printf("%s\n", ip_string);
}
void to_string_ipv6(struct sockaddr_in6 *addr)
{
    char ip_string[INET6_ADDRSTRLEN];

    inet_ntop(
        AF_INET6,
        &(addr->sin6_addr),
        ip_string,
        INET6_ADDRSTRLEN
    );

    printf("%s\n", ip_string);
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
                printf("%s IPv4: ", ifa->ifa_name);
                to_string_ipv4((struct sockaddr_in *)ifa->ifa_addr);
            }
            else if (ifa->ifa_addr->sa_family == AF_INET6) {
                printf("%s IPv6: ", ifa->ifa_name);
                to_string_ipv6((struct sockaddr_in6 *)ifa->ifa_addr);
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

    if (getifaddrs(&ifaddr) == -1) {
        return;
    }

    ifa = ifaddr;
    while (ifa != NULL) {

        if (ifa->ifa_addr != NULL) {

            if (strcmp(ifa->ifa_name, ifname) == 0) {

                if (ifa->ifa_addr->sa_family == AF_INET) {
                    printf("%s IPv4: ", ifa->ifa_name);
                    to_string_ipv4((struct sockaddr_in *)ifa->ifa_addr);
                }
                else if (ifa->ifa_addr->sa_family == AF_INET6) {
                    printf("%s IPv6: ", ifa->ifa_name);
                    to_string_ipv6((struct sockaddr_in6 *)ifa->ifa_addr);
                }
            }
        }

        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifaddr);
}

