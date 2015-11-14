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

#pragma mark - BASIC CONFIG

//#define SERVER "127.0.0.1"ç
//#define BUFLEN 512  //Max length of buffer
//#define SEND_PORT 8888   //The port on which to send data
//#define PORT_RECEIVE 8888   //The port on which to listen for incoming data

#pragma mark - MESSAGE SENDING

typedef struct RouterUp{
    struct sockaddr_in si_other;
    int s,port,idNumber;//=sizeof(si_other);
    socklen_t slen;
    char buf[MAX_USER_MSG_SIZE];
    char message[MAX_USER_MSG_SIZE];
    char destination_IP[15];
}RouterUp;

RouterUp initUpClient(RouterUp up);
void sendMessage(RouterUp up);
void closeUp(RouterUp up);
void muerte(char *s);

#pragma mark - MESSAGE RECEIVING

typedef struct RouterDown{
    struct sockaddr_in si_me, si_other;
    int s, i,port,idNumber;// = sizeof(si_other) , ;
    ssize_t recv_len;
    socklen_t slen;
    char buf[MAX_USER_MSG_SIZE];
    
}RouterDown;

RouterDown initDownClient(RouterDown down);
void * startDownListen();
void routerDidReceiveMessage(char message[1025]);
void closeDown(RouterDown down);
pthread_t prepareForDownload(RouterDown down);
#pragma mark - ROUTER ASSEMBLY

typedef struct SelfRouter{
    int idNumber;
    RouterUp upload;
    RouterDown download;
}SelfRouter;



#endif /* defined(____RouterIO__) */
