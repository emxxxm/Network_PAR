#ifndef CS_DATA_LINK_H
#define CS_DATA_LINK_H

#include "protocol.h"

uint16_t get_hash(const frame_t *frame);
int datalink_send(unsigned short sock, packet_t* pkt, FILE* outfile);
int datalink_recvACK(unsigned short sock, frame_t* frame);
int datalink_recv(unsigned short sock, packet_t* packet, FILE* &outfile);
int datalink_send_ACK(unsigned short sock, frame_t* frame);
int datalink_send_ACK_packet(unsigned short sock, ack_packet_t* packet);
int datalink_recvACK_packet(unsigned short sock, ack_packet_t* packet);

#endif // guard
