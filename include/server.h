#ifndef SERVER_H
#define SERVER_H
//STRUCTURES


struct f_clients;
struct args;
struct sockaddr_in init_serv_addr(int portnumber);

//INITIALISATION

int do_socket();
void do_bind(int socketnumber, struct sockaddr_in adr);
int do_listen(int socketnumber, int lognumber);
int do_accept(int socketnumber, struct sockaddr_in client);


//THREADS
void * fn_client (void* args);

#endif
