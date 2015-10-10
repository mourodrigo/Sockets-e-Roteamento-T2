//
//  main.c
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 01/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#include "main.h"




//========================================
#pragma mark - VISUAL INTERFACE
//========================================

int routerCount,linkCount;

void printRouter(router r){
    printf("ID: %d | Porta: %d | IP: %s\n",r.id,r.port,r.ip);
}

void printRouters(router r[routerCount]){
    for (int x=0; x<routerCount; x++) { //teletar
        printRouter(r[x]);
    }
}

void printlink(linkr l){
    printf("FROM: %d | TO: %d | COST: %d\n",l.from,l.to,l.cost);
}
void printLinks(linkr l[linkCount]){
    for (int x=0; x<linkCount; x++) {
        printlink(l[x]);
    }
}

void interface(struct router routers[MAX_ROUTERS],struct linkr links[MAX_LINKS]){

    printf("\n\n=========================\\n\n");
    printf("\n1 - Iniciar chat \n2 - Exibir enlaces \n3 - Exibir roteadores \n4 - Exibir caminhos \n0 - Sair\\n\n");
    printf("\n\n=========================\\n\n");
    int option=-1;
    while (option!=0) {
        scanf("%d",&option);
        switch (option) {
            case 1:
                chat();
                break;
            case 2:
                printLinks(links);
                break;
            case 3:
                printRouters(routers);
                break;
                
        }
    }
}


//int getIndexOfRouter(router r, struct router routers[MAX_ROUTERS]){
//    
//}




int main(int argc, const char * argv[]) {
    
//#if defined(MAC_OS_X_VERSION_MIN_REQUIRED)
//    printf("Mac");
//#endif

//    char *new_str;
//    asprintf(&new_str,"%s%s",argv[0],PATH_ROUTER_FILE);
//    printf("Hello there! Executing from: \n %s \n",new_str);

    if (argc<2) {
            printf("\n Usage: ./main <router id>\n");
    }else{
        printf("INIATING ROUTER ID: %s \n",argv[1]);
        
        //Web and routing structures
        struct router routers[MAX_ROUTERS];
        struct linkr links[MAX_LINKS];
        
        //Config file reading
        routerCount = readRouters(PATH_ROUTER_FILE, routers);
        linkCount = readLinks(PATH_LINKS_FILE, links);

        
        
        
        SelfRouter self;
        
        sleep(1);
        pthread_t download_Singleton = prepareForDownload(self.download);
        self.upload=initUpClient(self.upload,"192.168.25.210");
//        startDownListen();
        
        while (1) {
            
            printf("\nmsg: ");
            scanf("%s",self.upload.message);
            sendMessage(self.upload);
//            sleep(1);
        }
    
        //    add_links(linkCount, links);
        //    startDijkstra();
        //

        //    closeUp(r);
        
//        self.download = initDownClient(self.download);
//        startDownListen(self.download);
//        closeDown(self.download);
        
    }
    
    

    return 0;
}
