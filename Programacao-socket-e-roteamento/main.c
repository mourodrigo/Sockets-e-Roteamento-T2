//
//  main.c
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 01/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#include "main.h"

int routerCount,linkCount;

void printRouter(router r){
    printf("ID: %d | Porta: %d | IP: %s\n",r.id,r.port,r.ip);
}
void printlink(linkr l){
    printf("FROM: %d | TO: %d | COST: %d\n",l.from,l.to,l.cost);
}

//int getIndexOfRouter(router r, struct router routers[MAX_ROUTERS]){
//    
//}


int main(int argc, const char * argv[]) {
    // insert code here...

#if defined(MAC_OS_X_VERSION_MIN_REQUIRED)
    printf("Mac");
#endif

    char *new_str;
    asprintf(&new_str,"%s%s",argv[0],PATH_ROUTER_FILE);

    
    printf("Hello there! Executing from: \n %s \n",new_str);
    struct router routers[MAX_ROUTERS];
    struct linkr links[MAX_LINKS];
    
    routerCount = readRouters(PATH_ROUTER_FILE, routers);
    linkCount = readLinks(PATH_LINKS_FILE, links);

    for (int x=0; x<routerCount; x++) {
        printRouter(routers[x]);
    }
    
    for (int x=0; x<linkCount; x++) {
        printlink(links[x]);
    }

//    add_links(linkCount, links);
//    startDijkstra();
//    
//    serverTcpStart();
//    clientUdpStart();

    
//    RouterUp r;
//    r=initUpClient(r);
//    
//    while (1) {
//        printf("msg: ");
//        scanf("%s",r.message);
//        sendMessage(r);
//    }
    
    
    //    closeUp(r);
    
    RouterDown down;
    down = initDownClient(down);
    startDownListen(down);
    closeDown(down);

    return 0;
}
