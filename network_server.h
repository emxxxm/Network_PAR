#ifndef CS_NETWORK_SERVER_H
#define CS_NETWORK_SERVER_H

#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int network_rcv_packet(unsigned short sock, int &finish, FILE* &outlog);
int network_sendACK(unsigned short sock);

#endif
