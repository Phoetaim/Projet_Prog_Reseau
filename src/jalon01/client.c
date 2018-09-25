#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


// FUNCTIONS
struct sockaddr_in get_addr_info(char* hostname, int portnumber) {
  struct sockaddr_in sock_host;
  memset(& sock_host, '\0', sizeof(sock_host));
  sock_host.sin_family = AF_INET;
  sock_host.sin_port = htons(portnumber);
  inet_aton(hostname, & sock_host.sin_addr);
  return sock_host;
}

int do_socket(){
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1)
    perror("do_socket is wrong");
  return sock;
}

void do_connect(int sock, struct sockaddr_in address){
  int connexion = connect(sock, (struct sockaddr *) & address, sizeof(address));
  if(connexion == -1)
    perror("do_connect is wrong");
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

void handle_client_message(int sock, char * message) {
  sendline(sock, message, 300);
}

/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */
/* ----------------------------------------------------------------- */

int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

//get address info from the server
struct sockaddr_in address  = get_addr_info(argv[1], atoi(argv[2]));

int s;
char* str;
char* received;

for (;;) {

  //get the socket
  s = do_socket();

  //connect to remote socket
  do_connect(s, address);

  str = malloc(300*sizeof(char));

  //get user input$
  fgets(str, 300*sizeof(char), stdin);

  //send message to the server
  handle_client_message(s, str);

  //receive response from the server
  received = malloc(300*sizeof(char));
  readline(s, received, 300);

  // close the connection if \quit
  if (strncmp(str, "\\quit", 5) == 0) {
    if (strncmp(str, "\\quit ", 6) == 0) {
      puts(received);
    }
    close(s);
    free(str);
    break;
  }

   //display the response
   puts(received);

   //close the socket
   close(s);
  }

 return 0;


}
