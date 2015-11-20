//
//  Bellman-ford.h
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 19/11/15.
//  Copyright © 2015 mourodrigo. All rights reserved.
//

#ifndef Bellman_ford_h
#define Bellman_ford_h

#include <stdio.h>
#include "Defaults.h"

#define INFINITY 999
#define NODOS 1000

typedef struct {
    int source;
    int dest; //destino
    int weight; //peso
} Edge;

void BellmanFord(Edge edges[], int edgecount, int nodecount, int source);

#endif /* Bellman_ford_h */
