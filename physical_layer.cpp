#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "physical_layer.h"
#include "error.h"
#include <iostream>
#include <errno.h>

//Author: Xiaoman Xu
int physical_send(int sock, frame_t* frame)
{
    int retval;
    

    if ((retval = send(sock, frame, sizeof(frame_t), 0)) != sizeof(frame_t)) 
        DieWithError("Sending frame %u of length %lu failed with errno %d", frame->sequence_num, frame->length, errno);
    //log_info("A chunk was sent!")
    return retval;
}

//Author: Xiaoman Xu
int physical_recv(int sock, frame_t* frame)
{
    size_t pos;
    size_t frame_size;
    int rcv_size;
    frame_size = sizeof(frame_t);

    for(pos = 0; pos < frame_size; pos += rcv_size)
    {
        if((rcv_size = recv(sock, frame, frame_size - pos, 0)) <= 0)
        {
            return rcv_size;
        }
    }
    //log_info("A chunk was recieved!")
    return frame_size;
}

//Author: Xiaoman Xu
int physical_create_socket()
{
	int sock;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    return sock;    

}

//Author: Xiaoman Xu
int physical_connect(unsigned short sock, char* servName, unsigned short servPort)
{
	struct hostent *he;
	struct sockaddr_in servAddr;
	int connect_status;
    if((he = gethostbyname(servName)) == NULL)
    {
        DieWithError("gethostbyname error");
        return 2;
    }
    memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
    servAddr.sin_family      = AF_INET;             /* Internet address family */
    memcpy(&servAddr.sin_addr, he -> h_addr_list[0], he->h_length);   /* Server IP address */
    servAddr.sin_port        = htons(servPort); /* Server port */
    
    connect_status = connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0;
 
    return connect_status;
}

//Author: Xiaoman Xu
int physical_bind(unsigned short servSock, unsigned short servPort)
{
	struct sockaddr_in servAddr; 
	 /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));   /* Zero out structure */
    servAddr.sin_family = AF_INET;                /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    servAddr.sin_port = htons(servPort);      /* Local port */
    /* Bind to the local address */
    int bind_status = bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr));
	return bind_status;
       
}

//Author: Xiaoman Xu
int physical_listen(int servSock, unsigned int max_pending_clients)
{
	int listen_status = listen(servSock, max_pending_clients);
	return listen_status;
}
//Author: Xiaoman Xu
int physical_accept(unsigned short servSock)
{
	struct sockaddr_in clntAddr;
	unsigned int clntLen = sizeof(clntAddr);
	
	int accept_status =  accept(servSock, (struct sockaddr *) &clntAddr, 
                               &clntLen);
	printf("Accpeting client %s\n", inet_ntoa(clntAddr.sin_addr));
	return accept_status;
}
