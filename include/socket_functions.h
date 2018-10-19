#ifndef SOCKET_FUNCTIONS_H
#define SOCKET_FUNCTIONS_H

struct sockaddr_in init_serv_addr(int portnumber);

//INITIALISATION

int do_socket();
void do_bind(int socketnumber, struct sockaddr_in adr);
int do_listen(int socketnumber, int lognumber);
int do_accept(int socketnumber, struct sockaddr_in * client);

#endif
