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



//initialize the server
struct sockaddr_in init_serv_addr(int portnumber) {
	struct sockaddr_in sock_host;
	sock_host.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_host.sin_family = AF_INET;
	sock_host.sin_port = htons(portnumber);
	return sock_host;
}

//create a socket
int do_socket() {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
	perror("do_socket is wrong");
	return sock;
}


//bind the socket with an address
void do_bind(int socketnumber, struct sockaddr_in adr) {
	int b = bind(socketnumber, (struct sockaddr *) & adr, sizeof(adr));
	if (b == -1)
	perror("do_bind is wrong");
}


//listen on a socket
int do_listen(int socketnumber, int lognumber) {
	int l = listen(socketnumber, lognumber);
	if (l == -1)
	perror("do_listen is wrong");
	return l;
}


//accept a connection
int do_accept(int socketnumber, struct sockaddr_in  * client) {
	struct sockaddr_in client_tmp = *client;
	int size = sizeof(struct sockaddr_in);
	int a = accept(socketnumber, (struct sockaddr *)&client_tmp, &size);
	if (a == -1)
	perror("do_accept is wrong");
	client->sin_addr = client_tmp.sin_addr;
	client->sin_port = client_tmp.sin_port;
	return a;
}