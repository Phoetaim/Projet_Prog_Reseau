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
#include "../../include/socket_functions.h"
#include "../../include/server.h"
#include "../../include/list_client.h"

int NB_CLIENTS;


//STRUCTURES

//Structure of arguments
struct args {
	struct list_client * list_client;
	int l;
};

struct f_clients {
	pthread_t thread_client [20];
};//List  of the threads

//FUNCTIONS

//read the content of a socket
void readline(int fd, char * str, size_t maxlen) {
	ssize_t count = read(fd, str, maxlen);
	if(count == -1)
	perror("readline is wrong");
	while (count < maxlen)
	count += read(fd, str, maxlen-count);
}

//write on a socket
void sendline(int fd, char * str, size_t maxlen) {
	ssize_t count = write(fd, str, maxlen);
	if(count == -1)
	perror("sendline is wrong");
	while (count < maxlen)
	count += write(fd, str, maxlen-count);
}


//read
void do_read(int socketnumber, char * file) {
	readline(socketnumber, file, 300);
}

//write
void do_write(int socketnumber, char * file) {
	sendline(socketnumber, file, 300);
}

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
}
/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

int main(int argc, char** argv) {

	if (argc != 2){
		fprintf(stderr, "usage: ./SERVER port\n");
		return 1;
	}

	//create the socket
	int s = do_socket();
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

	//init the serv_add structure
	struct sockaddr_in address = init_serv_addr(atoi(argv[1]));
	struct list_client * list_client = new_list_client();
	struct sockaddr_in addr_client;
	struct f_clients *ff = malloc(20*sizeof(struct f_clients));
	int l = 0;

	//perform the binding
	do_bind(s, address);

	//specify the socket to be a server socket and listen for at most 20 concurrent client
	do_listen(s, 20);
	NB_CLIENTS = 0;
	puts("Waiting for connections...");

	while(1) {

		l = do_accept(s, addr_client);
		NB_CLIENTS ++;

		//check if the maximal number of connections is reached
		if (NB_CLIENTS > 20) {
			puts("Maximal connections reached");
			do_write(l, "2");
			close(l);
			NB_CLIENTS --;
		} else {
			do_write(l, "1");
			add_client(list_client, l, addr_client);
			struct args * arg = malloc(sizeof(struct args));
			arg->list_client = list_client;
			arg->l = l;
			pthread_create (& ff->thread_client [NB_CLIENTS], NULL, fn_client, arg);
		}
		//give the number of connections
		fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
		fflush(stdout);
	}

	free(ff);
	free(list_client);
	//clean up server socket
	close(s);

}
