#include "../../include/list_client.h"

struct clients {
	char * ip_address;
	int port_number;
	char * pseudo;
	time_t time_co;
	int socket_number;
	struct clients * next;
}; //List of the clients

struct list_client {
	struct clients * head;
};

// FUNCTIONS

struct list_client * new_list_client(){
	struct list_client * list_client = malloc(sizeof(struct list_client));
	list_client->head = NULL;
	return list_client;
}

void add_client (struct list_client * list_client, int l, struct sockaddr_in client) {

	struct clients * head_client = list_client->head;
	struct clients * new_client = malloc(sizeof(struct clients));
	char * ip_address = inet_ntoa(client.sin_addr);
	char * pseudo = "Find my pseudo";
	new_client->ip_address = malloc(sizeof(ip_address));
	strcpy(new_client->ip_address,ip_address);
	new_client->port_number = ntohs(client.sin_port) ;
	new_client->time_co = time(NULL);
	new_client->next = head_client;
	new_client->socket_number = l;
	new_client->pseudo = malloc(20*sizeof(char));
	strcpy(new_client->pseudo, pseudo);
	list_client->head = new_client;
}

void suppr_client (struct list_client * list_client, int l) {

	struct clients * head_client = list_client->head;
	if (head_client == NULL){
		printf("Erreur : no clients connected\n");
		exit(EXIT_FAILURE);
	}
	
	if (head_client->socket_number == l){
		list_client->head = head_client->next;
	}
	else{
		struct clients * ex_client = malloc(sizeof(struct clients));
			while(head_client->next->socket_number != l)
				head_client = head_client->next;
			ex_client = head_client->next;
			head_client->next = head_client->next->next;
	}
}

struct clients * find_client(struct list_client * list_client,int l){
	if (list_client == NULL){
		printf("Erreur : no clients connected\n");
		exit(EXIT_FAILURE);
	}
	struct clients * clients = list_client->head;
	while(clients->socket_number != l)
		clients = clients->next;
	return clients;
}

char * who (struct list_client * list_client) {
	struct clients * head_client = list_client->head;
	char * pseudo_members = malloc(300*sizeof(char));
	char * prefix = malloc(50*sizeof(char));
	strcat(pseudo_members,"Online users are :\n");
	while (head_client != NULL){
		strcpy(prefix, "\t- ");
		strcat(prefix, head_client->pseudo);
		strcat(prefix, "\n");
		strcat(pseudo_members,prefix);
		head_client=head_client->next;
	}
	return pseudo_members;		
}

char * whois (struct list_client * list_client, char * pseudo) {
	struct clients * head_client = list_client->head;
	char * info_client = malloc(300*sizeof(char));
	int size = strlen(pseudo);
	strcpy(pseudo + (size-1)*sizeof(char), "\0");
	
	while (head_client != NULL && strcmp(head_client->pseudo, pseudo) != 0){
		head_client = head_client->next;
	}
	if (head_client == NULL)
		strcat(info_client,"There is no user with this pseudo");
	else {
		
		char * time = ctime(&(head_client->time_co));
		size = strlen(time);
		strcpy(time + (size-1)*sizeof(char), "\0");
		sprintf(info_client,"%s is connected since %s with IP address %s and port number %d\n", pseudo, time, head_client->ip_address, head_client->port_number);
	}
	return info_client;
}

char * clients_get_pseudo(struct clients * clients){
	return clients->pseudo;
}

void clients_set_pseudo(struct clients * clients, char * pseudo){
	int size = strlen(pseudo);
	strcpy(pseudo + (size-1)*sizeof(char), "\0");
	strcpy(clients->pseudo, pseudo);
}
