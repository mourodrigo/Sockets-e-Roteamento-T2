//
//  Dijkstra.h
//  
//
//  Created by Rodrigo Bueno Tomiosso on 04/10/15.
//
//

#ifndef ____Dijkstra__
#define ____Dijkstra__

#include <stdio.h>
#include "Defaults.h"
void dijkstra(int vertices,int origem,int destino,int *custos);
void add_links(int linksCount, struct linkr links[MAX_LINKS]);
void limpar(void);
void cabecalho(void);
void add(void);
void procurar(void);
int startDijkstra();
#endif /* defined(____Dijkstra__) */
