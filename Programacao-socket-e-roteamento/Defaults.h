//
//  Defaults.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 01/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#ifndef Programacao_socket_e_roteamento_Defaults_h
#define Programacao_socket_e_roteamento_Defaults_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


//CONFIG FILE
#define PATH_ROUTER_FILE "/Users/macmouro/Developer/programa-o-socket-e-roteamento/Programacao-socket-e-roteamento/roteador.config"
#define PATH_LINKS_FILE "/Users/macmouro/Developer/programa-o-socket-e-roteamento/Programacao-socket-e-roteamento/enlaces.config"

#define ROUTER_FILE_NAME "roteador.config"
#define LINKS_FILE_NAME "enlaces.config"

//MAX SIZE FOR ARRAYS
#define MAX_ROUTERS 50
#define MAX_LINKS 50
#define SENDING_BUFFER_SIZE 50

#define MAX_PACKAGE_SIZE 999999
#define MAX_USER_MSG_SIZE 1000
#define MAX_PATH_STRING_SIZE 1000
#define MAX_HEADER_SIZE 256

//ROUTING CONFIGS
#define SEND_TRIES 5

//DEBUG
#define DEBUG_LEVEL_1
#define DEBUG_LEVEL_2
#define DEBUG_LEVEL_3
#define DEBUG_LEVEL_4

//SOCKET PORTS
#define SENDING_RANGE_MAX_PORT 8899
#define SENDING_RANGE_MIN_PORT 8800
#define RECEIVING_RANGE_MAX_PORT 8799
#define RECEIVING_RANGE_MIN_PORT 8700

//PACKAGE STATUS
#define PACKAGE_STATUS_EMBRYO 0
#define PACKAGE_STATUS_READY 1
#define PACKAGE_STATUS_SENDING 2
#define PACKAGE_STATUS_SENT 3

//REQUEST STATUS
#define REQUEST_STATUS_ERROR 0
#define REQUEST_STATUS_SEND_NO_ANSWER 1
#define REQUEST_STATUS_OK 2

//STRUCTS
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
    int nodes;
}linkr;

#endif
