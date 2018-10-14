#ifndef LIST_CLIENT_H
#define LIST_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>

struct list_client;

struct clients;

struct list_client * new_list_client();

void add_client (struct list_client * list_client, int l, struct sockaddr_in client);

void suppr_client (struct list_client * list_client, int l);

struct clients * find_client(struct list_client * list_client,int l);

char * who (struct list_client * list_client);

char * whois (struct list_client * list_client, char * pseudo);

char * clients_get_pseudo(struct clients * clients);

void clients_set_pseudo(struct clients * clients, char * pseudo);


#endif
