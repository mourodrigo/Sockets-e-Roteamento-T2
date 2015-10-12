
//
//  main.c
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 01/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#include "main.h"






//======================================================
#pragma mark - ROUTING MANAGE
//======================================================
RouterDown _down;
int routerCount,linkCount;

void muerte(char *s)
{
    perror(s);
    exit(1);
}

router routerOfIndex(int indx, router r[routerCount]){
    for (int x=0; x<routerCount; x++) { //teletar
        if (r[x].id==indx) {
            return r[x];
        }
    }
    router null;
    null.id=-1;
    return null;
}


char * routerToString(router r){
    char *router_str;
    asprintf(&router_str,"%d@%s",r.id,replace(r.ip, "\n", ""));
    
    char *router_str_with_port;
    asprintf(&router_str_with_port,"%s:%d|",router_str,r.port);
    return router_str_with_port;
}


router stringToRouter(char *s){
    char **a,**b,*string;
    router r;

    strcpy(string, s);

    a = str_split(string, '@');
    r.id = atoi(a[0]);
    
    b = str_split(a[1], ':');
    strcpy(r.ip, b[0]);
    r.port = atoi(b[1]);

    return r;
}


//======================================================
#pragma mark - UPLOAD
//======================================================

RouterUp initUpClient(RouterUp up){
    
    //    if (!up.slen) {
#ifdef DEBUG_LEVEL_3
    printf("Init RouterUP\n");
#endif
    up.slen = sizeof(up.si_other);
    
    if ( (up.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        muerte("socket");
    }
    
    memset((char *) &up.si_other, 0, sizeof(up.si_other));
    up.si_other.sin_family = AF_INET;
    up.si_other.sin_port = htons(up.port);
    
    
    if (inet_aton(up.destination_IP , &up.si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    //    }
    return up;
}
void sendMessage(RouterUp up){
    
    if (sendto(up.s, up.message, strlen(up.message) , 0 , (struct sockaddr *) &up.si_other, up.slen)==-1)
    {
        muerte("sendto()");
    }
    
    //receive a reply and print it
    //clear the buffer by filling null, it might have previously received data
    memset(up.buf,'\0', MAX_USER_MSG_SIZE);
    
    //try to receive some data, this is a blocking call
    if (recvfrom(up.s, up.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &up.si_other, &up.slen) == -1)
    {
        muerte("recvfrom()");
    }
    
#ifdef DEBUG_LEVEL_3
    printf("Confirmação de recebimento: %s",up.buf);
#endif
    
}

void closeUp(RouterUp up){
    close(up.s);
#ifdef DEBUG_LEVEL_3
    printf("Up Client Closed\n");
#endif
}












//======================================================
#pragma mark - DOWNLOAD
//======================================================

pthread_t prepareForDownload(RouterDown down){
    _down = initDownClient(down);
    pthread_t download_Singleton;
    
    pthread_create(&download_Singleton, NULL, startDownListen, (void*)&down);
    
    //    startDownListen(down);
    return download_Singleton;
}

RouterDown initDownClient(RouterDown down){
    
    down.slen = sizeof(down.si_other);
    
    //create a UDP socket
    if ((down.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        muerte("socket");
    }
    
    // zero out the structure
    memset((char *) &down.si_me, 0, sizeof(down.si_me));
    
    down.si_me.sin_family = AF_INET;
    down.si_me.sin_port = htons(down.port);
    down.si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(down.s , (struct sockaddr*)&down.si_me, sizeof(down.si_me) ) == -1)
    {
        muerte("bind");
    }
    
    
    
    
    return down;
}


void startDownListen(){
    
    //keep listening for data
#ifdef DEBUG_LEVEL_3
    printf("Router %d listening for data on port %d\n",_down.idNumber,_down.port);
#endif
    while(1)
    {
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(_down.buf,'\0', MAX_USER_MSG_SIZE);
        
        //try to receive some data, this is a blocking call
        if ((_down.recv_len = recvfrom(_down.s, _down.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &_down.si_other, &_down.slen)) == -1)
        {
            muerte("recvfrom()");
        }
        
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(_down.si_other.sin_addr), ntohs(_down.si_other.sin_port));
        printf("Data: %s\n" , _down.buf);
        
        //now reply the client with the same data
        if (sendto(_down.s, _down.buf, _down.recv_len, 0, (struct sockaddr*) &_down.si_other, _down.slen) == -1)
        {
            muerte("sendto()");
        }
    }
}

void closeDown(RouterDown down){
    close(down.s);
    
#ifdef DEBUG_LEVEL_3
    printf("Down Client Closed\n");
#endif
}











//========================================
#pragma mark - VISUAL INTERFACE
//========================================


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

router chooseDestination(struct router routers[MAX_ROUTERS]){
    printRouters(routers);
    router r;
    r.id=-1;
    while (r.id<0) {
        
        printf("\nEscolha o id do destinatário: ");
        int indx;
        scanf("%d",&indx);
        r=routerOfIndex(indx, routers);
    }
    return r;
}

void chat(struct router destination,SelfRouter self_router){

    RouterUp request;
    char message[MAX_USER_MSG_SIZE];
    request= upRequest(destination, message);
    request=initUpClient(request);
    sleep(1);
    while (1) {
        
        printf("\nmsg: ");
        scanf("%s",message);
        
        strcpy(request.message, message);
        sendMessage(request);
    }
    
//    upRequest(destination, <#char *message#>)
};


void interface(struct router routers[MAX_ROUTERS]/*,struct linkr links[MAX_LINKS]*/,SelfRouter self_router){

    printf("\n\n=========================\n\n");
    printf("\n1 - Iniciar chat \n2 - Exibir enlaces \n3 - Exibir roteadores \n4 - Exibir caminhos \n0 - Sair\n\n");
    printf("OPÇÃO: ");
    int option=-1;
    while (option!=0) {
        scanf("%d",&option);
        router r;
        switch (option) {
                case 1:
                r = chooseDestination(routers);
                chat(r,self_router);
                break;
            case 2:
//                printLinks(links);
                break;
            case 3:
                printRouters(routers);
                break;
            case 4:
                procurar();
                break;
        }
    }
}


RouterUp upRequest(router destination, char message[MAX_USER_MSG_SIZE]){
    RouterUp request;
    strcpy(request.destination_IP, destination.ip);
    request.idNumber = destination.id;
    request.port = SEND_PORT;
    strcpy(request.message, message);
    return request;
}





//========================================
#pragma mark - MAIN
//========================================

char * composeHeader(linkr l,struct router routers[MAX_ROUTERS]){
    char *line="";

    char** tokens;

    char *path_to_split;
    strcpy(path_to_split, l.path);
    tokens = str_split(path_to_split, '-');

    for (int x = 0; l.nodes; x++) {
        router r = routerOfIndex(atoi(tokens[x]), routers);
        asprintf(&line,"%s%s",line,routerToString(r));
    }

    return line;
    //    r.id = atoi(tokens[0]);
//    r.port = atoi(tokens[1]);

    
}

int main(int argc, const char * argv[]) {

    if (argc<2) {
            printf("\n Usage: ./main <router id>\n");
    }else{
//        printf("STARTING ROUTER ID: %s \n",argv[1]);
        
        
        //WEB AND ROUTING STRUCTURES
        struct router routers[MAX_ROUTERS];
        struct linkr links[MAX_LINKS];
        
        //CONFIG FILE READING
        routerCount = readRouters(PATH_ROUTER_FILE, routers);
        linkCount = readLinks(PATH_LINKS_FILE, links);
        
        //GATEWAY CONFIG INITIALIZATION
        router self_router = routerOfIndex(atoi(argv[1]), routers);
        SelfRouter self;
        self.download.port = self_router.port;
        self.idNumber = self.download.idNumber = self_router.id;
        
        //SINGLETON FOR DOWNLOAD/LISTENING DATA
        pthread_t download_Singleton = prepareForDownload(self.download);

        //ROUTING PATHS
        add_links(linkCount, links,routerCount);

        
        struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS];
        prepareRoutingPaths(linkGraph);
//
        
        composeHeader(linkGraph[2][6],routers);
        
        
        if (strcmp(argv[2], "v")==0) {
            sleep(1); //preparation for singleton init
            interface(routers/*, links*/,self);
        }else{
            printf("STARTING ROUTING MODE ID: %s \n",argv[1]);
        }

        
        

        
        
        
        
        
        
        
        
        
        
        
//        self.upload=initUpClient(self.upload);

        
        //        startDownListen();
    //    strcpy(up.destination_IP, destination_IP);
        

        
        
//        while (1) {
//            
//            printf("\nmsg: ");
//            scanf("%s",self.upload.message);
//            sendMessage(self.upload);
////            sleep(1);
//        }
    
        
        
//        closeDown(_down);
//        
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
