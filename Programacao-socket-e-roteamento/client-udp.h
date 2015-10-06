//
//  client-udp.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 05/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#ifndef Programacao_socket_e_roteamento_client_udp_h
#define Programacao_socket_e_roteamento_client_udp_h

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data


void __die(char *s);
int clientUdpStart(void);

#endif
