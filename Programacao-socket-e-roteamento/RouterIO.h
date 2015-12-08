//
//  RouterIO.h
//  
//
//  Created by Rodrigo Bueno Tomiosso on 06/10/15.
//
//

#ifndef ____RouterIO__
#define ____RouterIO__

#define _GNU_SOURCE

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

#pragma mark - MESSAGE SENDING

typedef struct Package{
    int localId;
    int destinationId;
    char destinationIP[15];
    int ttl;
    int type;
    char senderIP[15];
    char message[MAX_USER_MSG_SIZE];
    int status;
    int port;
    int packageId;
    int senderPort;
}Package;


typedef struct uploadSocket{
    struct sockaddr_in si_other;
    int s,port;//=sizeof(si_other);
    socklen_t slen;
    char buf[MAX_USER_MSG_SIZE]; //teletar?
    char message[MAX_USER_MSG_SIZE]; //teletar
    char destination_IP[15];
    int requestId;
    Package package;
    int timeoutnsec;
}uploadSocket;

typedef struct downloadSocket{
    struct sockaddr_in si_me, si_other;
    int s, i,port,idNumber;// = sizeof(si_other) , ;
    ssize_t recv_len;
    socklen_t slen;
    char buf[MAX_USER_MSG_SIZE];
    
}downloadSocket;

typedef struct router{
    int id;
    int port;
    char ip[15];
}router;

typedef struct linkr{
    int from;
    int to;
    int cost;
    char path[MAX_PATH_STRING_SIZE];
    int isDirectlyConnected;
    int ttl;
}linkr;

typedef struct connections{
    int selfID;
    struct router selfRouter;
    struct router routerList[MAX_ROUTERS];
    struct linkr linksList[MAX_LINKS];
    int routerCount, linksCount;
    struct uploadSocket uploadSocket;
    struct downloadSocket downloadSocket;
    struct linkr routingTable[MAX_LINKS][MAX_LINKS];
}connections;


void * startDownListen();
void routerDidReceiveMessage(char message[1025]);
#pragma mark - ROUTER ASSEMBLY


#endif /* defined(____RouterIO__) */
