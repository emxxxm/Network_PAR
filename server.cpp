//Author: Xiaoman Xu
//This code comes from D&C textbook TCPEchoServer.c
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>
#include <iostream>
#include "physical_layer.h"
#include "error.h"
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define FORK 1

void HandleTCPClient(int clntSocket);   /* TCP client handling function */

//Author: Jake Merdish
void child_handler(int sig)
{
    DieWithError("Child terminated, following suit");
}

//Author: Xiaoman Xu
int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
   
    unsigned short servPort;     /* Server port */
    //signal(SIGCHLD, child_handler);
    
    pid_t pid;
    if (argc != 1)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s \n", argv[0]);
        exit(1);
    }

    servPort = 2000;  /* Define a local port*/

    /* Create socket for incoming connections */
    if ((servSock = physical_create_socket()) < 0)
        DieWithError("Failed to create socket");

    if(physical_bind(servSock, servPort) < 0)
      	DieWithError("Failed to bind");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    while(true) /* Keeping running until handled a packet*/
    {
        /* Wait for a client to connect */
        if ((clntSock = physical_accept(servSock)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        if((pid = fork()) < 0)
        {
            DieWithError("Failed to fork.");
        }
        else if(pid == 0)
        {
            HandleTCPClient(clntSock);
            exit(0);
        }
    }
    close(servSock);
    exit(0);    
}
