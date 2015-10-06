//
//  RouterIO.h
//  
//
//  Created by Rodrigo Bueno Tomiosso on 06/10/15.
//
//

#ifndef ____RouterIO__
#define ____RouterIO__

#include <stdio.h>
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "Defaults.h"

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data
#define PORT_RECEIVE 8888   //The port on which to listen for incoming data

#pragma mark - MESSAGE SENDING

typedef struct RouterUp{
    struct sockaddr_in si_other;
    int s, slen;//=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
}RouterUp;

RouterUp initUpClient(RouterUp up);
void sendMessage(RouterUp up);
void closeUp(RouterUp up);
void muerte(char *s);

#pragma mark - MESSAGE RECEIVING

typedef struct RouterDown{
    struct sockaddr_in si_me, si_other;
    
    int s, i, recv_len,slen;// = sizeof(si_other) , ;
    char buf[BUFLEN];
}RouterDown;

RouterDown initDownClient(RouterDown down);
void startDownListen(RouterDown down);
void routerDidReceiveMessage(char message[1025]);
void closeDown(RouterDown down);
#endif /* defined(____RouterIO__) */
