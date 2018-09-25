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


// ERRORS
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//FUNCTIONS
struct sockaddr_in init_serv_addr(int portnumber) {
 struct sockaddr_in sock_host;
 memset(& sock_host, '\0', sizeof(sock_host));
 sock_host.sin_family = AF_INET;
 sock_host.sin_port = htons(portnumber);
 inet_aton("127.0.0.1", & sock_host.sin_addr);
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
    do_listen(s, 20);
    struct sockaddr_in client;
    
    for (;;)
    {

        //accept connection from client
        int l = do_accept(s, client);
        fprintf(stdout, "%d", l);

        //read what the client has to say
        char* tmp = malloc(300*sizeof(char));
        do_read(l, tmp);

        //we write back to the client


        if (strncmp(tmp, "\\quit", 5) == 0) {
          if (strncmp(tmp, "\\quit ", 6) == 0)
            do_write(l, (tmp+5*sizeof(char)));
          close(l);
        }
        else {
          do_write(l, tmp);
        }
        //clean up client socket
    }

    //clean up server socket
    close(s);

    return 0;
}
