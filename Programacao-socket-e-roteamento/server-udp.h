//
//  server-udp.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 06/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#ifndef Programacao_socket_e_roteamento_server_udp_h
#define Programacao_socket_e_roteamento_server_udp_h

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

void die(char *s);
int serverUdpStart(void);

#endif
