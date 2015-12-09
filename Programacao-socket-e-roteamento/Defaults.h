//
//  Defaults.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 01/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#ifndef Programacao_socket_e_roteamento_Defaults_h
#define Programacao_socket_e_roteamento_Defaults_h

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "limits.h"

//CONFIG FILE
#define PATH_ROUTER_FILE "/Users/mourodrigo/Developer/Sockets-e-Roteamento-T2/Programacao-socket-e-roteamento/roteador.config"
#define PATH_LINKS_FILE "/Users/mourodrigo/Developer/Sockets-e-Roteamento-T2/Programacao-socket-e-roteamento/enlaces.config"

#define ROUTER_FILE_NAME "roteador.config"
#define LINKS_FILE_NAME "enlaces.config"

//MAX SIZE FOR ARRAYS
#define MAX_ROUTERS 20
#define MAX_LINKS 20
#define ROUTING_BUFFER_SIZE 30

#define MAX_PACKAGE_SIZE 999
#define MAX_USER_MSG_SIZE 1000
#define MAX_PATH_STRING_SIZE 1000
#define MAX_HEADER_SIZE 256

//ROUTING CONFIGS
#define SEND_TRIES 1
#define MAX_TTL 5
#define MAX_TTL_BROADCAST 2
#define MAX_LINK_TTL 15

//SOCKET PORTS
#define SENDING_RANGE_MAX_PORT 8899
#define SENDING_RANGE_MIN_PORT 8884
#define RECEIVING_RANGE_MAX_PORT 8799
#define RECEIVING_RANGE_MIN_PORT 8700

//PACKAGE STATUS
#define PACKAGE_STATUS_PROCESSING 0
#define PACKAGE_STATUS_READY 1
#define PACKAGE_STATUS_SENDING 2
#define PACKAGE_STATUS_SENT 3
#define PACKAGE_STATUS_DONE 4


#define PACKAGE_TYPE_BROADCAST 255
#define PACKAGE_TYPE_BROADCAST_ACK 256
#define PACKAGE_TYPE_BROADCAST_ERROR 404
#define PACKAGE_TYPE_MESSAGE 111
#define PACKAGE_TYPE_FORWARD 222
#define PACKAGE_TYPE_ACK 333

#define PACKAGE_STATUS_READY 1
#define PACKAGE_STATUS_SENDING 2
#define PACKAGE_STATUS_SENT 3
#define PACKAGE_STATUS_ACK 4
#define PACKAGE_STATUS_DEAD 5

#define BROADCAST_CODE_LINK_ERROR 404

//REQUEST STATUS
#define REQUEST_STATUS_ERROR 0
#define REQUEST_STATUS_SEND_NO_ANSWER 1
#define REQUEST_STATUS_OK 2

#define REQUEST_TIMEOUT_NSEC 100

#endif
