#include <netinet/in.h>

/* Liste des fonctions de ifshow.c utilisables par l'agent */
void show_all_interfaces(int numSortie);
void show_one_interface(const char *ifname, int numSortie);