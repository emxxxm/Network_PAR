#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "error.h"
#include "physical_layer.h"
#include "data_link.h"
#include <sys/select.h>
#include <sys/time.h>

#define GODDESS (1)
#define GODDESS_DATA_TOLERANCE (6)
#define GODDESS_ACK_TOLERANCE (11)
static int data_seq = 0;
static int ack_seq = 0;

uint16_t frame_send_seq = 1;
uint16_t frame_rcv_seq = 1;
uint16_t frame_sent = 0;
uint16_t ack_frame_send_seq = 1;

int num_of_frame_sent = 0;
int packet_sent = 1;
int num_of_retransmission = 0;
int num_of_goodACK = 0;
int num_of_errorACK = 0;
//Author: Jake Merdish
//error simulation
void flip_frame(frame_t* frame)
{
	int break_it = 0;
    if (frame->type == FRAME_ACK) {

       if (++ack_seq == GODDESS_ACK_TOLERANCE) {
           break_it = 1;
           ack_seq = 0;
       }
    } else if (frame->type == FRAME_DATA) {
       if (++data_seq == GODDESS_DATA_TOLERANCE) {
           break_it = 1;
           data_seq = 0;
       }
    }
    break_it &= GODDESS; // adds a disable function

    if (break_it) {
        int bit =rand()%sizeof(frame->check);
        frame->check ^= 0x01 << bit;
    }
}

//Author: Jake Merdish
//checksum
uint16_t get_hash(const frame_t* frame){
    uint16_t hash = 0;
    size_t i;
    if (frame->type == FRAME_DATA){
    for (i = 0; i<frame->length; i++){
        hash ^= frame->payload[i]<<(i%2); //deals with double length
    }
    } else if (frame->type == FRAME_ACK){
        hash = frame->sequence_num;
    }
    return hash;
}

//Author: Xiaoman Xu
int datalink_send(unsigned short sock, packet_t *packet, FILE* outfile)
{
	frame_t frame, ack_frame;
	uint8_t len = (uint8_t)FRAME_SIZE;
	unsigned int readpos = 0;
	int transmit_time = 0;
	size_t packet_size = sizeof(packet_t);

	//While still more to send in a packet, split the packet into
	//frames to transmit along physcial layer
	while(transmit_time < (int)packet_size/FRAME_SIZE + 1)
	{
		uint16_t frame_expected;
		frame_expected = frame_send_seq;
		frame.type = FRAME_DATA;
		frame.is_end = (transmit_time == (packet_size/FRAME_SIZE));
		
		//Want to make sure get the right length
		if(frame.is_end)
		{
			len = packet_size % FRAME_SIZE;	
		}
		
		memcpy(frame.payload, ((char*) packet) + readpos, len);
		
		frame.length = len;
		frame.sequence_num = frame_expected;
		
		//checksum
        frame.check = get_hash(&frame);

        //set a timer
        fd_set rfds; 
		struct timeval tv; 
		int retval;
		FD_ZERO(&rfds); 
		FD_SET(sock,&rfds);
		tv.tv_sec=0;
		tv.tv_usec=50000;

        if(frame_expected == frame_sent)
        {
        	//if the frame_expected is the frame already sent, record the same frame retransmitted
        	fprintf(outfile, "Client Data Link: frame %d of packet %d retransmited.\t(frame %d)\n",
        		transmit_time + 1, packet_sent, frame_expected);
        	num_of_retransmission++;
        }
        else
        {
        	//do error simulation 
        	flip_frame(&frame);
        	fprintf(outfile, "Client Data Link: frame %d of packet %d sent.\t(frame %d)\n",
        		transmit_time + 1, packet_sent, frame_expected);
        	
        } 
        
        //send along physcial layer;
		physical_send(sock, &frame);
		num_of_frame_sent ++;
		frame_sent = frame_send_seq;
		bool valid = false;
		
		//while not valid, keep receiving
		while (!valid)
		{
			int frame_ack = 1;
			//start the timer;
			retval = select(sock+1, &rfds,0, 0, &tv);
			if(retval == 0)
			{
				fprintf(outfile, "Client Data Link: time out after frame %d of packet %d sent.\t(frame %d)\n",
        		transmit_time + 1, packet_sent, frame_expected);
				break;
			}
			else if(retval < 0)
			{
				DieWithError("Fail to set timer");
			}
			else{
				FD_ISSET(0, &rfds);
			}
			
			frame_ack = datalink_recvACK(sock, &ack_frame);
			fprintf(outfile, "Client Data Link: ACK frame %d of packet %d received.\t(ACK frame %d)\n",
        		transmit_time + 1, packet_sent, ack_frame.sequence_num);
			
			//check whether the frame received is valid.
			valid = ack_frame.check == get_hash(&ack_frame);
			if(!valid)
			{
				fprintf(outfile, "Client Data Link: ACK frame %d of packet %d received in error.(ACK frame %d)\n",
        		transmit_time + 1, packet_sent, ack_frame.sequence_num);
        		num_of_errorACK ++;
			}
			else
			{
				num_of_goodACK ++;
			}

			if(!frame_ack)
				valid = false;
        	//log_debug("ack_frame %d is valid? %d", frame.sequence_num, valid);
        }
        //if the frame acked succesfully, ready to send the next one
        if(valid)
        {
        	
			readpos += len;
			frame_send_seq++;
			transmit_time++;
        }

        if(packet->last_photo && packet ->is_end && frame.is_end)
        {
        	fprintf(outfile, "\nTotal frame sent: %d\nTotal retransmission: %d\nTotal goodACK: %d\nTotal badACK: %d\n\n",
        	 num_of_frame_sent, num_of_retransmission, num_of_goodACK, num_of_errorACK);
        }

	}

	//record the packet sent number
	packet_sent++;
	return packet_size;
}

/** Author: Xiaoman Xu
 * 	client datalink layer 
 *	receive ack
 **/
int datalink_recvACK(unsigned short sock, frame_t *frame)
{

	//call physcial layer receive
    physical_recv(sock, frame);

    //check whether it's ACK FRAME or Data frame
    if(frame->type == FRAME_ACK)
    {
    	return 1;
 	}	
	else
	{
		return 0;
	}
}

/** Author: Xiaoman Xu
 *  get the ACK packet from 
 *  server network layer and send
 *	it as data frame to client
 **/
int datalink_send_ACK_packet(unsigned short sock, ack_packet_t* packet)
{
	int packet_size = sizeof(ack_packet_t);
	if(packet->type == PACKET_ACK)
	{

		frame_t frame;
		frame.type = FRAME_DATA;
		frame.is_end = 1;
		
		int len = packet_size % FRAME_SIZE;
		//log_debug("packet size is %d", packet_size);
		//log_debug("len is %d", len);

		memcpy(frame.payload, ((char*) packet), len);
		frame.length = len;
		physical_send(sock, &frame);

	}
	return packet_size;
	
}
/** Author: Xiaoman Xu
 *	client data link layer
 *	receive a data frame and 
 * 	put it in the packet
 * 	and forward it to client
 *	network layer
 **/
int datalink_recvACK_packet(unsigned short sock, ack_packet_t * packet)
{
	frame_t frame;
	physical_recv(sock, &frame);
	if(frame.type == FRAME_DATA)
	{
		memcpy(((char*)packet), frame.payload, frame.length);
	}
	else
	{
	//	log_debug("Receive Wrong type of ACK");
		//DieWithError("Expect a data frame containing a network ACK packet");
	}
	return sizeof(packet_t);
}
/** Author: Xiaoman Xu
 *	Server side data link layer
 *	receives frames and 
 * 	reassembles it into a packet
 *	and forward to to server netword layer
 **/
int datalink_recv(unsigned short sock, packet_t* packet, FILE* &outlog)
{
	int frame_num = 0;
	size_t recv_at = 0;
	int recv_size = 0;
	frame_t frame, ack_frame;;
    bool valid, repeat = false; 

	size_t packet_size = sizeof(packet_t);

	//while still more to receive
	while(recv_at < packet_size)
	{
		uint16_t frame_expected;
		frame_expected = ack_frame_send_seq;
        valid = false;

        //if the data frame received is not valid,
        //keep receiving
        while (!valid) {
            if((recv_size = physical_recv(sock, & frame)) != sizeof(frame_t))
            {
                DieWithError("Server DataLink: Physical layer receive different size %d than expected", recv_size);
            }

            //if the data frame seq num is the frame expected, 
            //then copy the data from the payload
            if(frame.sequence_num == frame_expected)
            {
            	
            	repeat = false;
            	memcpy(((char*)packet) + recv_at, frame.payload, frame.length);
            	
            }

            //selse the data frame is repeated.
            else
            {
            	repeat = true;
            }
            // check whether the data frame is valid
            valid = frame.check == get_hash(&frame);
            if(!valid)
            	fprintf(outlog, "Server data link layer: data frame %d of packet %d received in error (frame %d)\n",
            		frame_num, packet_sent, frame_expected);
        }

        //open the log file if not already
        if(outlog == NULL)
        {
        	char logname [20];
        	sprintf(logname, "server_%c.log", packet->filename[0]);
        	outlog = fopen(logname, "w");
        }
		ack_frame.type = FRAME_ACK;
		ack_frame.length = 0;
		ack_frame.sequence_num = ack_frame_send_seq;
	    ack_frame.check = get_hash(&ack_frame);
	    //send the ACK frame to client
		if(!repeat)
		{
			frame_num ++;
			fprintf(outlog, "Server Data Link: data frame %d of packet %d received.(frame %d)\n",
        		frame_num, packet_sent, frame_expected);
			recv_at += frame.length;
			flip_frame(&ack_frame);
			ack_frame_send_seq++;

		}
		else
		{
			fprintf(outlog, "Server data link: data frame %d of packet %d repeated!(frame %d)\n",
				frame_num, packet_sent, frame_expected);
		}
		
		datalink_send_ACK(sock, &ack_frame);
		fprintf(outlog, "Server data link layer: ACK frame %d of packet %d sent.(frame %d)\n",
			frame_num, packet_sent, frame_expected);

		if(frame.is_end&&repeat)
		{
			return 0;
		}	
	}
	packet_sent++;
    
	return sizeof(packet_t);
}

//Author: Xiaoman Xu

//Data link layer send ACK frame
int datalink_send_ACK(unsigned short sock, frame_t* frame)
{
	size_t sent;
	
	if((sent = physical_send(sock, frame))!= sizeof(frame_t))
		DieWithError("Data link send ACK: send different size (%lu) than expected (%lu)", sent, sizeof(frame_t));
	
	return 0;
}

