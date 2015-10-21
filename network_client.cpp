#include <iostream>
#include "network_client.h"
#include "protocol.h"
#include "data_link.h"
#include "error.h"
//Author: Xiaoman Xu
int network_send_packet(unsigned short sock, char* photo_name, int finish, FILE* outfile)
{
	FILE* photo = NULL;
	packet_t packet;
	photo = fopen(photo_name, "rb"); //open the photo
	char id_to_send[2];
	photo_name += 5;
	memcpy(id_to_send, photo_name, 2);
	uint8_t read_buffer[PACKET_SIZE];
	if(photo == NULL)
	{
		DieWithError("Cannot open file %s", photo_name);
	}
	
	size_t read_size;
	
	while((read_size = fread(read_buffer, 1, PACKET_SIZE, photo)) > 0)
	{
		packet.last_photo = finish;
		memcpy(packet.filename, photo_name, 2);
		packet.is_end = (read_size < PACKET_SIZE);
		memcpy(packet.chunk, read_buffer, read_size);
		packet.length = read_size;
		fprintf(outfile, "Client: Network packet forwarded\n");
		datalink_send(sock, &packet, outfile); // send the packet to datalink layer
		
		network_recvACK(sock); //wait to receive ACK packet
		fprintf(outfile, "Client: Network received ACK packet\n\n");
	}
	return read_size;

	
}

//Author: Xiaoman Xu
int network_recvACK(unsigned short sock)
{
	ack_packet_t ACK_packet;
	
	datalink_recvACK_packet(sock, &ACK_packet);
	return sizeof(ack_packet_t);
}
