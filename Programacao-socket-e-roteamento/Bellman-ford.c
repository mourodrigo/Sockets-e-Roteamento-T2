//
//  Bellman-ford.c
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 19/11/15.
//  Copyright © 2015 mourodrigo. All rights reserved.
//

#include "Bellman-ford.h"

int distancia[NODOS];

void BellmanFord(Edge edges[], int edgecount, int nodecount, int source) {
    int i,j,trocou;

    for (i = 0; i < nodecount; i++) {
        distancia[i] = INFINITY;
    }
    distancia[source]=0;
    for (i = 0; i < nodecount; i++) {
        trocou = 0;
        for (j = 0; j < edgecount; j++) {
            if (distancia[edges[j].dest] > distancia[edges[j].source] + edges[j].weight) {
                distancia[edges[j].dest] = distancia[edges[j].source] + edges[j].weight;
                trocou=1;
            }
        }
        // se nenhuma iteração teve efeito, futuras iterações estão dispensadas
        if (trocou==0) break;
    }
    // usado somente para detectar ciclos negativos (dispensável)
    for (i = 0; i < edgecount; i++) {
        if (distancia[edges[i].dest] > distancia[edges[i].source] + edges[i].weight) {
            printf("\nCiclo negativo de pesos de arestas detectado");
            break;
        }
    }
    for (i = 0; i < nodecount; i++) {
        printf("\nA distancia mais curta entre os nodos %d e %d eh %d",source, i, *distancia);
        }
}

//int main (void){
//        // Este caso de teste produzira as distancias entre o nodo 0 e os outros nodos
//        Edge Arestas[10] = {{0, 1, 5}, {0,2, 8}, {0,3, -4}, {1,0, -2},
//        {2, 1, -3}, {2,3, 9}, {3,1, 7}, {3,4, 2},
//        {4,0, 6}, {4,2, 7}};
//        // BellmanFord(Estrutura, arestas, vertices,origem);
//        BellmanFord(Arestas, 10, 5, 0);
//    return 0;
//}