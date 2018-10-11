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

int NB_CLIENTS;


//STRUCTURES

struct list_clients {
  char * ip_address;
  int port_number;
  char * pseudo;
  time_t time_co;
  int socket_number;
  struct list_clients * next;
}; //List of the clients

struct f_clients {
  pthread_t thread_client [20];
};//List  of the threads

struct args {
  struct list_clients * list;
  int l;
}; //Structure of arguments for the threads

//FUNCTIONS

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
int do_accept(int socketnumber, struct sockaddr_in client) {
  int size = sizeof(struct sockaddr_in);
  int a = accept(socketnumber, (struct sockaddr *)&client, &size);
  if (a == -1)
  perror("do_accept is wrong");
  return a;
}


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

// WHOS FUNCTIONS

struct list_clients * add_client (struct list_clients * list_client, int l, struct sockaddr_in client) {
  if (list_client == NULL) {
    struct list_clients * clients_connected = malloc(sizeof(struct list_clients));
    clients_connected->ip_address = inet_ntoa(client.sin_addr);
    clients_connected->port_number = client.sin_port;
    clients_connected->time_co = time(NULL);
    clients_connected->next = NULL;
    clients_connected->socket_number = l;
    clients_connected->pseudo = NULL;
    return clients_connected;
  } //if(list_client == NULL)
  else {
    struct list_clients* temp = list_client;
    struct list_clients* next = malloc(sizeof(struct list_clients));
    while (temp->next != NULL)
      temp = temp->next;

    next->ip_address = inet_ntoa(client.sin_addr);
    next->port_number = client.sin_port;
    next->time_co = time(NULL);
    next->next = NULL;
    next->socket_number = l;
    next->pseudo = NULL;
    temp->next = next;

    return list_client;
  }//else
}

int suppr_client (struct list_clients * clients_connected, int l) {
  if (clients_connected == NULL){
    printf("Erreur : no clients connected\n");
    exit(EXIT_FAILURE);
  }
  struct list_clients *element = clients_connected;
  struct list_clients *temp = clients_connected->next;

  if (clients_connected->socket_number == l){
    if (temp == NULL){
      clients_connected = NULL;
      return 0;
    }
    else {
      while(temp->next != NULL){
        element = temp;
        temp = temp->next;
      }
      clients_connected->ip_address = temp->ip_address;
      clients_connected->port_number = temp->port_number;
      clients_connected->time_co = temp->time_co;
      clients_connected->socket_number = temp->socket_number;
      clients_connected->pseudo = temp->pseudo;
      element->next = NULL;
      free(temp);
      free(element);
      return 1;
    }
  }//if (clients_connected->socket_number == l)

  while(temp != NULL && temp->socket_number != l) {
    element = temp;
    temp = temp->next;
  }
  element->next = temp->next;
  free(temp);
  free(element);
  return 2;
}

/*void who (struct list_client clients) {

}

void whois (struct list_client clients, char * pseudo) {

}*/

//THREADS

void * fn_client (void* ss) {

  struct args * tempo= malloc(sizeof(struct args));
  tempo = (struct args *) ss;

  struct list_clients * clients_connected = tempo->list;
  int l =  tempo->l;

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
      fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
      fflush(stdout);
      free(serv);
      free(tmp);
      pthread_exit(NULL);
    }
    do_write(l, "[Server] : Please login with /nick <pseudo>");
    do_read(l, tmp);
  }
  strcpy(serv, "[Server] : Welcome on the chat ");
  strcat(serv, tmp+6*sizeof(char));
  do_write(l, serv);
  clients_connected->pseudo = tmp+6*sizeof(char);

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
      fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
      fflush(stdout);
      break;
    }

    else if(strncmp(tmp, "/nick ", 6) == 0) {
      strcpy(serv, "[Server] : Your new pseudo is ");
      strcat(serv, tmp+6*sizeof(char));
      do_write(l, serv);
    } else {
      //we write back to the client
      strcpy(serv, "[Server] : ");
      strcat(serv, tmp);
      do_write(l, serv);
    }
  }
  free(tmp);
  free(serv);
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
  struct list_clients * clients_connected = malloc(sizeof(struct list_clients));
  clients_connected = NULL;
  struct sockaddr_in client;
  struct f_clients *ff = malloc(20*sizeof(struct f_clients));
  int l = 0;

  //perform the binding
  do_bind(s, address);

  //specify the socket to be a server socket and listen for at most 20 concurrent client
  do_listen(s, 20);
  NB_CLIENTS = 0;
  puts("Waiting for connections...");

  while(1) {

    l = do_accept(s, client);
    NB_CLIENTS ++;

    //check if the maximal number of connections is reached
    if (NB_CLIENTS > 20) {
      puts("Maximal connections reached");
      do_write(l, "2");
      close(l);
      NB_CLIENTS --;
    } else {
      do_write(l, "1");

      clients_connected = add_client(clients_connected, l, client);
      struct args * arg = malloc(sizeof(struct args));
      arg->list = clients_connected;
      arg->l = l;
      pthread_create (& ff->thread_client [NB_CLIENTS], NULL, fn_client, arg);
    }

    //give the number of connections
    fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
    fflush(stdout);
  }

  free(ff);
  free(clients_connected);
  //clean up server socket
  close(s);

}
