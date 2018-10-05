#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

int NB_CLIENTS;


//FUNCTIONS

struct list_clients {
  char * ip_address;
  int port_number;
  char * pseudo;
  time_t time_co;
  int socket_number;
  struct list_clients * next;
};

struct f_clients {
  pthread_t thread_client [20];
};

struct sockaddr_in init_serv_addr(int portnumber) {
 struct sockaddr_in sock_host;
 sock_host.sin_addr.s_addr = htonl(INADDR_ANY);
 sock_host.sin_family = AF_INET;
 sock_host.sin_port = htons(portnumber);
 return sock_host;
}

int do_socket() {
 int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 if (sock == -1)
   perror("do_socket is wrong");
 return sock;
}

void do_bind(int socketnumber, struct sockaddr_in adr) {
 int b = bind(socketnumber, (struct sockaddr *) & adr, sizeof(adr));
 if (b == -1)
   perror("do_bind is wrong");
}

int do_listen(int socketnumber, int lognumber) {
 int l = listen(socketnumber, lognumber);
 if (l == -1)
   perror("do_listen is wrong");
 return l;
}

int do_accept(int socketnumber, struct sockaddr_in client) {
  int size = sizeof(struct sockaddr_in);
  int a = accept(socketnumber, (struct sockaddr *)&client, &size);
  if (a == -1)
    perror("do_accept is wrong");
  return a;
}

void readline(int fd, char * str, size_t maxlen) {
  ssize_t count = read(fd, str, maxlen);
  if(count == -1)
    perror("readline is wrong");
  while (count < maxlen) {
    count += read(fd, str, maxlen-count);
  }
}

void sendline(int fd, char * str, size_t maxlen) {
  ssize_t count = write(fd, str, maxlen);
  if(count == -1)
    perror("sendline is wrong");
  while (count < maxlen) {
    count += write(fd, str, maxlen-count);
  }
}

void do_read(int socketnumber, char * file) {
  readline(socketnumber, file, 300);
}

void do_write(int socketnumber, char * file) {
  sendline(socketnumber, file, 300);
}

// WHOS FUNCTIONS

struct list_clients * add_client (struct list_clients * list_clients, int l, struct sockaddr_in client) {
  if (list_clients == NULL) {
    struct list_clients * clients_connected = malloc(sizeof(struct list_clients));
    clients_connected->ip_address = inet_ntoa(client.sin_addr);
    clients_connected->port_number = inet_ntoa(client.sin_port);
    clients_connected->time_co = time(NULL);
    clients_connected->next = NULL;
    clients_connected->socket_number = l;
    return clients_connected;
  }
  else {
    struct list_clients* temp = malloc(sizeof(struct list_clients));
    *(temp->ip_address) = inet_ntoa(client.sin_addr);
    temp->port_number = inet_ntoa(client.sin_port);
    temp->time_co = time(NULL);
    temp->next = list_clients;
    temp->socket_number = l;
    return temp;
  }
}

struct list_clients * suppr_client (struct list_clients * clients_connected, int l) {
  if (clients_connected == NULL)
  {
    printf("Erreur : no clients connected\n");
    exit(EXIT_FAILURE);
  }
  if (clients_connected->socket_number == l)
    return clients_connected->next;

    struct list_clients *temp = clients_connected->next;
    struct list_clients *element = clients_connected;
    while(temp != NULL && temp->socket_number != l) {
      element = temp;
      temp = temp->next;
    }
    element->next = temp->next;
    free(temp);
    free(element);
    return clients_connected;
}


//THREADS

void * fn_client (void* ss) {

  struct list_clients * clients_connected = (struct list_clients *) ss;
  int l =  clients_connected->socket_number;
  char* serv;
  char* tmp = malloc(300*sizeof(char));
  do_read(l, tmp);
  while (strncmp(tmp, "/nick ", 5) != 0)
    do_write(l, "[Server] : Please login with /nick <pseudo>");
  serv = malloc(300*sizeof(char));
  strcpy(serv, "[Server] : Welcome on the chat ");
  strcat(serv, tmp+5*sizeof(char));
  do_write(l, serv);
  free(serv);

  strcpy(clients_connected->pseudo, tmp+5*sizeof(char));

  while(1) {
    //read what the client has to say
    do_read(l, tmp);

    //check if /quit
    if (strncmp(tmp, "/quit", 5) == 0) {
      if (strncmp(tmp, "/quit ", 6) == 0) {
          serv = malloc(300*sizeof(char));
          strcpy(serv, "[Server] : You will be terminated -> ");
          strcat(serv, tmp+6*sizeof(char));
          do_write(l, serv);
          free(serv);
        } else {
          do_write(l, "[Server] : You will be terminated");
        }
        close(l);
        NB_CLIENTS --;
        fprintf(stdout, "%d client(s) connected\n", NB_CLIENTS);
        fflush(stdout);
        break;
    }
    else {
      //we write back to the client
      serv = malloc(300*sizeof(char));
      strcpy(serv, "[Server] : ");
      strcat(serv, tmp);
      do_write(l, serv);
      free(serv);
    }
  }
  free(tmp);

  pthread_exit(NULL);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }

    //create the socket, check for validity!
    int s = do_socket();

    //init the serv_add structure
    struct sockaddr_in address = init_serv_addr(atoi(argv[1]));

    //perform the binding
    //we bind on the tcp port specified
    do_bind(s, address);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(s, 2);
    NB_CLIENTS = 0;
    puts("Waiting for connections...");

    struct list_clients * clients_connected = NULL;

    struct sockaddr_in client;

    struct f_clients *ff = malloc(sizeof(struct f_clients));

    while(1) {

      int l = do_accept(s, client);
      NB_CLIENTS ++;
      if (NB_CLIENTS > 2) {
        puts("Maximal connections reached");
        do_write(l, "2");
        close(l);
        NB_CLIENTS--;
      } else {
        do_write(l, "1");
        clients_connected = add_client(clients_connected, l, client);

        pthread_create (& ff->thread_client [NB_CLIENTS], NULL, fn_client, clients_connected);
    }

      fprintf(stdout, "%d client connected\n", NB_CLIENTS);
      fflush(stdout);



    }
    free(ff);

    //clean up server socket
    close(s);

}
