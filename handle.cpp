//Author: Xiaoman Xu
//This code comes from D&C textbook TCPHandleClient.c
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#define RCVBUFSIZE 32   /* Size of receive buffer */
#include "protocol.h"
#include "error.h"
#include "physical_layer.h"
#include "network_server.h"

//Author: Xiaoman Xu
void HandleTCPClient(int clntSocket)
{  
    log_info("Client connected and successfully forked");
    int finish = 0;
    FILE* outlog = NULL;

    //if not the last photo, keep waiting receiving
    while(!finish)
    {
    	network_rcv_packet(clntSocket, finish, outlog);
    	log_info("File recieved");
    }
    fclose(outlog);
    

    close(clntSocket);    /* Close client socket */
}
