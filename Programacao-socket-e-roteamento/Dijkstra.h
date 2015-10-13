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
void add_links(int linksCount, struct linkr links[MAX_LINKS],int routerCount);
void dijkstraPath(int vertices,int origem,int destino,int *custos,struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS]);
void prepareRoutingPaths(struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS]);

#endif /* defined(____Dijkstra__) */
