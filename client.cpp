#include <iostream>
#include <cstdlib> 
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <cstdio>     
#include <string.h>    
#include <unistd.h>     
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <ctype.h>
#include "error.h"
#include "physical_layer.h"
#include "network_client.h"
using namespace std;

//Author: Xiaoman Xu
int main(int argc, char* argv[])
{
	int sock;
	unsigned short servPort = 2000;
	int photo_cnt, photo_id, client_id, finish;
	
	char photo_name[20];
	//log_info("on %d", servPort);
	if(argc != 4)
	{
		DieWithError("Usage: %s <servermachine> <id> <nums_photos>", argv[0]);
	}
	char* logname = (char*) malloc(13 + strlen(argv[2]));
	sprintf(logname, "client_%s.log", argv[2]);
	FILE* outfile = fopen(logname, "w");

    struct timeval tv;
    gettimeofday(&tv, 0);   //record start time
    double t1 = tv.tv_sec + (double) tv.tv_usec/1000000;
	if ((sock = physical_create_socket()) < 0)
        DieWithError("socket() failed");
    
    if(physical_connect(sock, argv[1], servPort) < 0) //establish TCP connection 
    	DieWithError("Connect failed");

    photo_cnt = atoi(argv[3]);
    client_id = atoi(argv[2]);


    //sending photo
    for(photo_id = 1; photo_id <= photo_cnt; photo_id++)
    {
    	if(photo_id == photo_cnt)
        {
            finish = 1;
        }
        else finish = 0;
    	sprintf(photo_name, "%s%1d%1d.jpg", "photo", client_id, photo_id);
        //client network layer send packets
    	network_send_packet(sock, photo_name, finish, outfile);
    }

    gettimeofday(&tv, 0);
    double t2 = tv.tv_sec + (double) tv.tv_usec/1000000;
    fprintf(outfile, "Excution time for client%s: %.6fs", argv[2], t2 - t1);
    fclose(outfile);
    log_info("Client exited successfully");
}
