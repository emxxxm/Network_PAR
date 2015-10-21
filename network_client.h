#ifndef CS_NETWORK_CLIENT_H
#define CS_NETWORK_CLIENT_H

#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int network_send_packet(unsigned short sock, char* photo_name, int finish, FILE* outfile);
int network_recvACK(unsigned short sock);

#endif
