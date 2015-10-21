#ifndef CS_PROTOCOL_H
#define CS_PROTOCOL_H

#include <stdint.h>
#include <stdio.h>

#define PACKET_SIZE 256
#define FRAME_SIZE 130
#define PACKET_DATA 0x01
#define PACKET_ACK 0x02
//Author: Jake Merdish
struct ack_packet{
    char type;
};
typedef struct ack_packet ack_packet_t;
struct packet{
	char filename[2];
    char chunk[PACKET_SIZE];
    int length; // soft capped at 256
    uint8_t is_end;
    uint8_t last_photo;
};
typedef struct packet packet_t;

#define FRAME_DATA 0x01
#define FRAME_ACK 0x02


struct frame{
    char type;
    uint16_t sequence_num;
    uint16_t check;
    uint8_t is_end;
    char payload[FRAME_SIZE];
    uint8_t length; // soft capped at 130
};


typedef struct frame frame_t;

#endif // guard
