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
    int i,v, cont = 0;
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
    
    /* Mostra o Resultado da busca */
    printf("\tDe %d para %d: \t", origem, destino);
    linkGraph[origem][destino].from = origem;
    linkGraph[origem][destino].to = destino;
    
    if (min == HUGE_VAL) {
        linkGraph[origem][destino].cost = 0;

        printf("Nao Existe\n");
        printf("\tCusto: \t- \n");
    }
    else {
        i = destino;
        i = ant[i-1];
        
        char *path="";
        
        while (i != -1) {
            //   printf("<-%d",i+1);
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
            printf("%d->", tmp[i-1]);
            asprintf(&path,"%s%s",tmppath,"");
        }
        
        char *tmppath;
        asprintf(&tmppath,"%s-%d",path,destino);
        printf("%d-", tmp[i-1]);
        asprintf(&path,"%s%s",tmppath,"");

        
        printf("%d", destino);
        linkGraph[origem][destino].cost = dist[destino-1];
        strcpy(linkGraph[origem][destino].path, path);
        printf("\n\tCusto:  %d\n",(int) dist[destino-1]);
    }
//    return linkGraph;
}

void prepareRoutingPaths(struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS])
{
    int i, j;
    
    /* Azul */
    printf("{FONTE}33[36;1m");
    printf("Lista dos Menores Caminhos no Grafo Dado: \n");
    
    for (i = 1; i <= vertices; i++) {
        for (j = 1; j <= vertices; j++)
            //            dijkstra(vertices, i,j, custos);
            dijkstraPath(vertices, i, j, custos, linkGraph);
        printf("\n");
    }
    
    printf("<Pressione ENTER para retornar ao menu principal>\n");
    /* Volta cor nornal */
    printf("{FONTE}33[m");
}



void dijkstra(int vertices,int origem,int destino,int *custos)
{
    
    int i,v, cont = 0;
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
    
    /* Mostra o Resultado da busca */
    printf("\tDe %d para %d: \t", origem, destino);
    if (min == HUGE_VAL) {
        printf("Nao Existe\n");
        printf("\tCusto: \t- \n");
    }
    else {
        i = destino;
        i = ant[i-1];
        while (i != -1) {
            //   printf("<-%d",i+1);
            tmp[cont] = i+1;
            cont++;
            i = ant[i];
        }
        
        for (i = cont; i > 0 ; i--) {
            printf("%d -> ", tmp[i-1]);
        }
        printf("%d", destino);
        
        printf("\n\tCusto:  %d\n",(int) dist[destino-1]);
    }
}




void limpar(void)
{
    printf("{FONTE}33[2J"); /* limpa a tela */
    printf("{FONTE}33[1H"); /* poe o curso no topo */
}

void cabecalho(void)
{
    limpar();
    printf("Implementacao do Algoritmo de Dijasktra\n");
    printf("Comandos:\n");
    printf("\t d - Adicionar um Grafo\n"
           "\t r - Procura Os Menores Caminhos no Grafo\n"
           "\t CTRL+c - Sair do programa\n");
    printf(">>> ");
}

void add_links(int linksCount, struct linkr links[MAX_LINKS],int routerCount)
{
    int i, j;
    
//    do {
//        printf("\nInforme o numero de vertices (no minimo 2 ): ");
//        scanf("%d",&vertices);
//    } while (vertices < 2 );

    vertices = routerCount;
    
    if (!custos)
        free(custos);
    custos = (int *) malloc(sizeof(int)*vertices*vertices);
    for (i = 0; i <= vertices * vertices; i++)
        custos[i] = -1;
    
    printf("vertices %d - Entre com as Arestas:\n",vertices);
    linksCount--;
    do {
//        printf("\n\nlinkscount %d\n links[linksCount].from %d links[linksCount].to %d links[linksCount].cost %d\n\n", linksCount,links[linksCount].from,links[linksCount].to,links[linksCount].cost);
        
        do {
//            printf("Origem da aresta (entre 1 e %d ou '0' para sair): ", vertices);
//            scanf("%d",&origem);
            origem = links[linksCount].from;
        } while (origem < 0 || origem > vertices);
        
        if (origem) {
            do {
//                printf("Destino da aresta (entre 1 e %d, menos %d): ", vertices, origem);
//                scanf("%d", &destino);
                destino = links[linksCount].to;
            } while (destino < 1 || destino > vertices || destino == origem);
            
            do {
//                printf("Custo (positivo) da aresta do vertice %d para o vertice %d: ",
//                       origem, destino);
//                scanf("%d",&custo);
                custo = links[linksCount].cost;
            } while (custo < 0);
            
            custos[(origem-1) * vertices + destino - 1] = custo;
        }
        linksCount--;
    } while (linksCount);
}

void add(void)
{
    int i, j;
    
    do {
        printf("\nInforme o numero de vertices (no minimo 2 ): ");
        scanf("%d",&vertices);
    } while (vertices < 2 );
    
    if (!custos)
        free(custos);
    custos = (int *) malloc(sizeof(int)*vertices*vertices);
    for (i = 0; i <= vertices * vertices; i++)
        custos[i] = -1;
    
    printf("Entre com as Arestas:\n");
    do {
        do {
            printf("Origem da aresta (entre 1 e %d ou '0' para sair): ", vertices);
            scanf("%d",&origem);
        } while (origem < 0 || origem > vertices);
        
        if (origem) {
            do {
                printf("Destino da aresta (entre 1 e %d, menos %d): ", vertices, origem);
                scanf("%d", &destino);
            } while (destino < 1 || destino > vertices || destino == origem);
            
            do {
                printf("Custo (positivo) da aresta do vertice %d para o vertice %d: ",
                       origem, destino);
                scanf("%d",&custo);
            } while (custo < 0);
            
            custos[(origem-1) * vertices + destino - 1] = custo;
        }
        
    } while (origem);
}

void procurar(void)
{
    int i, j;
    
    /* Azul */
    printf("{FONTE}33[36;1m");
    printf("Lista dos Menores Caminhos no Grafo Dado: \n");
    
    for (i = 1; i <= vertices; i++) {
        for (j = 1; j <= vertices; j++)
            dijkstra(vertices, i,j, custos);
        printf("\n");
    }
    
    printf("<Pressione ENTER para retornar ao menu principal>\n");
    /* Volta cor nornal */
    printf("{FONTE}33[m");
}



int startDijkstra() {
    int i, j;
    char opcao[3], l[50];
    
    do {
        
        cabecalho();
        scanf("%s", &opcao);
        
        if ((strcmp(opcao, "d")) == 0) {
            add();
        }
        FLSH;
        
        if ((strcmp(opcao, "r") == 0) && (vertices > 0) ) {
            procurar();
            FLSH;
        }
        
    } while (opcao != "x"); 
    
    printf("\nAte a proxima...\n\n");
    
    return 0;
}