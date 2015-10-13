//
//  Dijkstra.c
//  
//
//  Created by Rodrigo Bueno Tomiosso on 04/10/15.
//
//

#include "Dijkstra.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FLSH gets(l)

int destino, origem, vertices = 0;
int custo, *custos = NULL;


void dijkstraPath(int vertices,int origem,int destino,int *custos,struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS])
{
    int i,v = 0, cont = 0;
    int *ant, *tmp;
    int *z;     /* vertices para os quais se conhece o caminho minimo */
    double min;
    double dist[vertices]; /* vetor com os custos dos caminhos */
    
    
    /* aloca as linhas da matriz */
    ant = calloc (vertices, sizeof(int *));
    tmp = calloc (vertices, sizeof(int *));
    if (ant == NULL) {
        printf ("** Erro: Memoria Insuficiente **");
        exit(-1);
    }
    
    z = calloc (vertices, sizeof(int *));
    if (z == NULL) {
        printf ("** Erro: Memoria Insuficiente **");
        exit(-1);
    }
    
    for (i = 0; i < vertices; i++) {
        if (custos[(origem - 1) * vertices + i] !=- 1) {
            ant[i] = origem - 1;
            dist[i] = custos[(origem-1)*vertices+i];
        }
        else {
            ant[i]= -1;
            dist[i] = HUGE_VAL;
        }
        z[i]=0;
    }
    z[origem-1] = 1;
    dist[origem-1] = 0;
    
    /* Laco principal */
    do {
        
        /* Encontrando o vertice que deve entrar em z */
        min = HUGE_VAL;
        for (i=0;i<vertices;i++)
            if (!z[i])
                if (dist[i]>=0 && dist[i]<min) {
                    min=dist[i];v=i;
                }
        
        /* Calculando as distancias dos novos vizinhos de z */
        if (min != HUGE_VAL && v != destino - 1) {
            z[v] = 1;
            for (i = 0; i < vertices; i++)
                if (!z[i]) {
                    if (custos[v*vertices+i] != -1 && dist[v] + custos[v*vertices+i] < dist[i]) {
                        dist[i] = dist[v] + custos[v*vertices+i];
                        ant[i] =v;
                    }
                }
        }
    } while (v != destino - 1 && min != HUGE_VAL);
    
    linkGraph[origem][destino].from = origem;
    linkGraph[origem][destino].to = destino;
    
    if (min == HUGE_VAL) {
        linkGraph[origem][destino].cost = 0;

    }
    else {
        i = destino;
        i = ant[i-1];
        
        char *path="";
        
        while (i != -1) {
            tmp[cont] = i+1;
            cont++;
            i = ant[i];
        }
        
        for (i = cont; i > 0 ; i--) {
            char *tmppath;
            if (i==cont) {
                asprintf(&tmppath,"%s%d",path,tmp[i-1]);
            }else{
                asprintf(&tmppath,"%s-%d",path,tmp[i-1]);
            }
            asprintf(&path,"%s%s",tmppath,"");
        }
        
        char *tmppath;
        asprintf(&tmppath,"%s-%d",path,destino);
        asprintf(&path,"%s%s",tmppath,"");

        
        linkGraph[origem][destino].cost = dist[destino-1];
        strcpy(linkGraph[origem][destino].path, path);

        linkGraph[origem][destino].nodes = cont;
        
    }
}

void prepareRoutingPaths(struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS])
{
    int i, j;
    
    for (i = 1; i <= vertices; i++) {
        for (j = 1; j <= vertices; j++)
            //            dijkstra(vertices, i,j, custos);
            dijkstraPath(vertices, i, j, custos, linkGraph);
        printf("\n");
    }
}

void add_links(int linksCount, struct linkr links[MAX_LINKS],int routerCount)
{
    int i;
    vertices = routerCount;
    
    if (!custos)
        free(custos);
    custos = (int *) malloc(sizeof(int)*vertices*vertices);
    for (i = 0; i <= vertices * vertices; i++)
        custos[i] = -1;
    
    linksCount--;
    do {
        
        do {
            origem = links[linksCount].from;
        } while (origem < 0 || origem > vertices);
        
        if (origem) {
            do {
                destino = links[linksCount].to;
            } while (destino < 1 || destino > vertices || destino == origem);
            
            do {
                custo = links[linksCount].cost;
            } while (custo < 0);
            
            custos[(origem-1) * vertices + destino - 1] = custo;
        }
        linksCount--;
    } while (linksCount);
}




