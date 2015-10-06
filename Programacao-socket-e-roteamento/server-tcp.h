//
//  server-tcp.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 05/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#ifndef Programacao_socket_e_roteamento_server_tcp_h
#define Programacao_socket_e_roteamento_server_tcp_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

void serverTcpStart();

#endif
