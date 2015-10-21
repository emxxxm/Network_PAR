#include <iostream>
#include "protocol.h"
#include "network_server.h"
#include "data_link.h"
#include "error.h"

int packet_recv = 1;

/** Author: Xiaoman Xu
 *	Server network layer
 *  receive a packet and 
 *	send an ACK packet
 */

int network_rcv_packet(unsigned short sock, int &finish, FILE* &outlog)
{
	packet_t packet;
	char filename[20], newfile[20];
	int rcv_size = 0;
	FILE* outfile = NULL;
	packet.is_end = 0;
	
	if(datalink_recv(sock, &packet, outlog) != sizeof(packet_t))
			DieWithError("Network receive: Failed to receive from datalink");
	
	fprintf(outlog, "Server Network received packet%d. \n", packet_recv);
	
	memcpy(filename, packet.filename, 2);
	
	sprintf(newfile, "photonew%c%c.jpg", filename[0], filename[1]);

	packet_recv++;

	if((outfile = fopen(newfile, "wb")) == NULL)
		DieWithError("Failed to open new file to write");
	

	fwrite(packet.chunk, 1, packet.length, outfile);
	/* Processed a packet and network layer sends an ACK packet */
	network_sendACK(sock);
	fprintf(outlog, "Server Network sent ACK packet\n\n");
	
	while(!packet.is_end)
	{
		
		if(datalink_recv(sock, &packet, outlog) != 0)
		{
			fwrite(packet.chunk, 1, packet.length, outfile);
			fprintf(outlog, "Server Network received packet%d. \n", packet_recv);
			fprintf(outlog, "Server Network sent ACK packet\n\n");
			packet_recv++;
		}
		
		/* Processed a packet and network layer sends an ACK packet */
		network_sendACK(sock);
		
		
		rcv_size += packet.length;
	}
	finish = packet.last_photo;
	fclose(outfile);
	fclose(outlog);
	
	return rcv_size;
}

//Author: Xiaoman Xu
int network_sendACK(unsigned short sock)
{
	ack_packet_t ACK_packet;
	ACK_packet.type = PACKET_ACK;
	datalink_send_ACK_packet(sock, &ACK_packet);
	return sizeof(ack_packet_t);
	
}
