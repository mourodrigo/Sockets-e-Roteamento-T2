//
//  main.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 06/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#ifndef Programacao_socket_e_roteamento_main_h
#define Programacao_socket_e_roteamento_main_h

#include "Defaults.h"
#include "FileIO.h"
#include "Dijkstra.h"

//#include "server-tcp.h"
//#include "server-udp.h"
//#include "client-tcp.h"
//#include "client-udp.h"

#include "RouterIO.h"

void printRouter(router r);
void printlink(linkr l);
int main(int argc, const char * argv[]);

#endif