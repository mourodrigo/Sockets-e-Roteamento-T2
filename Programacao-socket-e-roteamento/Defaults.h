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

#define PATH_ROUTER_FILE "/Users/mourodrigo/Developer/Programacao-socket-e-roteamento/Programacao-socket-e-roteamento/roteador.config"
#define PATH_LINKS_FILE "/Users/mourodrigo/Developer/Programacao-socket-e-roteamento/Programacao-socket-e-roteamento/enlaces.config"

#define MAX_ROUTERS 50
#define MAX_LINKS 50

#define DEBUG_LEVEL_1
#define DEBUG_LEVEL_2
#define DEBUG_LEVEL_3
#define DEBUG_LEVEL_4

#define MAX_PACKAGE_SIZE 999999
#define MAX_USER_MSG_SIZE 1000
#define MAX_PATH_STRING_SIZE 1000

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
}linkr;

typedef struct package{
    char msg[100];
    char to[15];
}package;

#endif
