#include <stdio.h>
#include <arpa/inet.h>
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