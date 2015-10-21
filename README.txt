Program 3
Author: Xiaoman Xu and Jake Merdish

Program contains:
client.cpp
data_link.cpp data_link.h
network_client.cpp network_client.h
network_server.cpp network_server.h
physical_layer.cp physcial_layer.h
protocol.h error.h error.cpp
Makefile

How to test:
1. Run make
2. start server by ./server first
3. log onto another ccc machine
	run ./client hostname(e.g. cccwork4.wpi.edu) clientId num_of_phot
	example would be ./client cccwork4.wpi.edu 1 5
4. To test whether the client can deal with multiple process
	start a different ccc machine, 
	run the same command but with different client id
	eg. ./client cccwork4.wpi.edu 2 5

5. The time used by client to transmit all photos and 
	the number of frame sent,
	the number of retransmission,
	the number of good ACK frame,
	the number of ACK frame with error
	are located at the very end of the client_id.log.

6. The log recorded which frame of which pakcet is 
	sent/receive/receive in error
	there are frame seq number appended and the end 
	of each log sentence to indicate its true
	sequence number.

7. The client can quit automatically, but the server may
	need to be shut down mannully by pressing 
	Ctrl-C

*Performance Notice*
1. The timeout setting is 50ms, so to transmit
	all the photos, it may take up to an hour.
2. To test quickly, four relatively small test photos are provided as
	photo31.jpg photo 32.jpg and 
	photo41.jpg and photo 42.jpg 
	This may server as a quicker demonstration to show that our code works.
	(still may need a few minutes though)

Design:
1. We have a packet struct and ACK packet struct,
	in regular packet we store the id of the client
	that server may know which client it is handling
2. When split packet into frames, we encapsualte the whole
	packet, so the size of struct packet is around 268,
	it may need up to 3 frames when transmit a packet.
3. We add other fields such as last_photo to indicate server 
	when to close the connection and length to indicate how
	much data we have.
4. We also develop a macro debug tool which will also generate an 
	output.log file. Since most debug info is commented out,
	there should only "File received" left to indicate a photo
	has been succefully received.

## Application Notes:
 - This program handles dropped packets, repeated packets, and packets with a corrupted checksum (for those not guaranteed good in the assignment).
 - The server forks itself for each client, creating a new logfile as soon as the client finishes its first packet (ie. when it receives the client id). Until that point, it uses 'server_master.log'.
 - The client uses select() for timeouts. 
 
## Application Layout:
 - Server.cpp handles the pre-fork behavior of the server, with the post-fork handler code in handle.cpp
 - Client.cpp handles the client main function.
 - Network_client and network_server handle the network layer for each
 - Data_link is unified across client and server. It also handles error throwing there instead of the physical layer, since some traffic is guaranteed good.
 - Physical_layer is just a TCP wrapper.
 - Error.cpp and error.h handle all of the log formatting and writing.

General Authorship:
There are authorship comment in each 
Jake Merdich:
 - Logging
 - Error simulation and checksum
 - Protocol header file
 
Xiaoman Xu:
 - main()
 - Network layer sending/receiving
 - Datalink layer sending/receiving
 - Physical layer simulation
 - Timer


