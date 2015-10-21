#ifndef CS_PYSCIAL_LAYER_H
#define CS_PYSCIAL_LAYER_H


#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "protocol.h"

int physical_create_socket();
int physical_connect(unsigned short sock, char* servName, unsigned short servPort);
int physical_bind(unsigned short servSock, unsigned short servPort);
int physical_listen(int sock, unsigned int max_pending_clients);
int physical_accept(unsigned short servSock);
int physical_send(int sock, frame_t* frame);
int physical_recv(int sock, frame_t* frame);

#endif
