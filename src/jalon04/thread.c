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
/*

//Structure of arguments
struct args {
	struct list_client * list_client;
	int l;
};

//THREADS

void * fn_client (void* args) {

	struct args * arg_tmp = (struct args *)args;
	struct list_client * list_client = arg_tmp->list_client;
	int l =  arg_tmp->l;
	struct clients * client_connected = find_client(list_client, l);

	char* serv = malloc(300*sizeof(char));
	char* tmp = malloc(300*sizeof(char));
	do_read(l, tmp);

	while (strncmp(tmp, "/nick ", 6) != 0) {
		if (strncmp(tmp, "/quit", 5) == 0) {
			if (strncmp(tmp, "/quit ", 6) == 0) {
				strcpy(serv, "[Server] : You will be terminated -> ");
				strcat(serv, tmp+6*sizeof(char));
				do_write(l, serv);
			} else {
				do_write(l, "[Server] : You will be terminated");
			}
			close(l);
			//give the number of connections
			NB_CLIENTS --;
			free(serv);
			free(tmp);
			fprintf(stdout,"Client %d terminated\n", l);
			fflush(stdout);
			fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
			fflush(stdout);
			pthread_exit(NULL);
		}
		do_write(l, "[Server] : Please login with /nick <pseudo>");
		do_read(l, tmp);
	}
	clients_set_pseudo(client_connected,tmp + 6*sizeof(char));
	fprintf(stdout,"Client %d is %s\n", l, clients_get_pseudo(client_connected));
	fflush(stdout);
	strcpy(serv, "[Server] : Welcome on the chat ");
	strcat(serv, clients_get_pseudo(client_connected));
	do_write(l, serv);

	while(1) {
		//read what the client has to say
		do_read(l, tmp);
		//check if /quit
		if (strncmp(tmp, "/quit", 5) == 0) {
			if (strncmp(tmp, "/quit ", 6) == 0) {
				strcpy(serv, "[Server] : You will be terminated -> ");
				strcat(serv, tmp+6*sizeof(char));
				do_write(l, serv);
			} else
			do_write(l, "[Server] : You will be terminated");
			close(l);

			//give the number of connections
			NB_CLIENTS --;
			fprintf(stdout,"Client %d (%s) terminated\n", l, clients_get_pseudo(client_connected));
			fflush(stdout);
			fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
			fflush(stdout);
			break;
		}

		else if(strncmp(tmp, "/nick ", 6) == 0) {
			clients_set_pseudo(client_connected, tmp + 6*sizeof(char));
			fprintf(stdout,"Client %d new pseudo is %s\n", l, clients_get_pseudo(client_connected));
			fflush(stdout);
			strcpy(serv, "[Server] : Your new pseudo is ");
			strcat(serv, clients_get_pseudo(client_connected));
			do_write(l, serv);
		} else {
			//we write back to the client
			if (strncmp(tmp, "/whois ", 7) == 0)
				tmp = whois(list_client,tmp + 7*sizeof(char));
			else if (strncmp(tmp, "/who", 4) == 0)
				tmp = who(list_client);
			strcpy(serv, "[Server] : ");
			strcat(serv, tmp);
			do_write(l, serv);
		}
	}
	suppr_client(list_client,l);
	free(tmp);
	free(serv);
	free(client_connected);
	//exit the thread

	pthread_exit(NULL);
}*/