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
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#include "Defaults.h"
#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data

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
#endif /* defined(____RouterIO__) */
