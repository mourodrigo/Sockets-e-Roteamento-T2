
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
int routerCount,linkCount, sendingBufferIndex, autoIncrementalLocalRequestId;
Package sendingBuffer[SENDING_BUFFER_SIZE];


void die(char *s)
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
    asprintf(&router_str_with_port,"%s:%d",router_str,r.port);
    return router_str_with_port;
}


router stringToRouter(char *s){
    char **a,**b;
    char string[MAX_USER_MSG_SIZE]="";
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

RouterUp initUpClient(RouterUp up){ //teletar

    
    
    up.slen = sizeof(up.si_other);
    
    if ( (up.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    memset((char *) &up.si_other, 0, sizeof(up.si_other));
    up.si_other.sin_family = AF_INET;
    up.si_other.sin_port = htons(up.port);
    
    
    if (inet_aton(up.destination_IP , &up.si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    return up;
}


RouterUp newSendRequestForPackage(Package p){

    RouterUp newRequest;
    newRequest.port = SENDING_RANGE_MIN_PORT;
    newRequest.requestId = 0;
    strcpy(newRequest.destination_IP, p.destinationIP);
    
    while (!newRequest.requestId) {
        newRequest.slen = sizeof(newRequest.si_other);
        if ( (newRequest.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
            die("socket");
        }
        
        memset((char *) &newRequest.si_other, 0, sizeof(newRequest.si_other));
        newRequest.si_other.sin_family = AF_INET;
        newRequest.si_other.sin_port = htons(newRequest.port);
        
        
        if (inet_aton(newRequest.destination_IP , &newRequest.si_other.sin_addr) == 0)
        {
            sprintf(stderr, "inet_aton() failed creating request with port id %d",newRequest.port);
            newRequest.port++;
        }else{
            newRequest.requestId = autoIncrementalLocalRequestId++;
        }
        
        

    }
    
    return newRequest;
}
void sendMessage(RouterUp up){
    
    int sendTries = SEND_TRIES;
    while (sendTries--) {
        
        struct timeval tv;
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        if (setsockopt(up.s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            perror("send Message Socket Error");
        }
        
        if (sendto(up.s, up.message, strlen(up.message) , 0 , (struct sockaddr *) &up.si_other, up.slen)==-1)
        {
            die("error sendto()");
        }
        
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(up.buf,'\0', MAX_USER_MSG_SIZE);
        
        //try to receive some data, this is a blocking call
        if (recvfrom(up.s, up.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &up.si_other, &up.slen) == -1)
        {
    #ifdef DEBUG_LEVEL_3
            //die("recvfrom()");
            printf("\nProblema ao enviar mensagem para %s:%d",up.destination_IP, up.port);
    #endif
        }else{
    #ifdef DEBUG_LEVEL_3
            printf("\n\nConfirmação de recebimento: %s:%d",up.destination_IP, up.port);
            break;
    #endif
        }
    }
    if (sendTries<1) {
        printf("\nDEU PAU MESMO, nao tem como enviar para %s:%d",up.destination_IP, up.port);
    }else{
        printf("tudo certo ;) %s:%d",up.destination_IP, up.port);
    }
    
}

void closeUp(RouterUp up){
    close(up.s);
#ifdef DEBUG_LEVEL_3
    printf("Up Client Closed\n");
#endif
}

void sendPackage(char *s){
    RouterUp request;
    char string[MAX_HEADER_SIZE];
    
    strcpy(string, s);
    char **a = str_split(string, '~');
    char **b = str_split(a[1], '|');

    router r = stringToRouter(b[0]);
    request= upRequest(r, s);
    request=initUpClient(request);
    sendMessage(request);
}



void addSendPackageToBuffer(Package p){
    sendingBuffer[sendingBufferIndex] = p;
    sendingBufferIndex++;
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
    _down = down;
    
    _down.slen = sizeof(_down.si_other);
    
    //create a UDP socket
    if ((_down.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    // zero out the structure
    memset((char *) &_down.si_me, 0, sizeof(_down.si_me));
    
    _down.si_me.sin_family = AF_INET;
    _down.si_me.sin_port = htons(_down.port);
    _down.si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(_down.s , (struct sockaddr*)&_down.si_me, sizeof(_down.si_me) ) == -1)
    {
        die("bind");
    }
    return _down;
}


void * startDownListen(void){ //listener to download data on thread
    
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
        
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 900000;
        if (setsockopt(_down.s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            perror("Error");
        }
        
        
        //try to receive some data, this is a blocking call
        if ((_down.recv_len = recvfrom(_down.s, _down.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &_down.si_other, &_down.slen)) == -1)
        {
//            printf("erro recvfrom()");
        }else{
            //print details of the client/peer and the data received
            printf("\nMENSAGEM RECEBIDA DE %s:%d\n", inet_ntoa(_down.si_other.sin_addr), ntohs(_down.si_other.sin_port));
            printf("\nConteúdo: %s\n" , _down.buf);
            
            //now reply the client with the same data
            if (sendto(_down.s, _down.buf, _down.recv_len, 0, (struct sockaddr*) &_down.si_other, _down.slen) == -1)
            {
                die("sendto()");
            }
            
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
#pragma mark - PACKAGE ROUTING
//========================================


void routing(SelfRouter self,RouterDown down,struct router routers[MAX_ROUTERS]){
    
    //keep listening for data
#ifdef DEBUG_LEVEL_3
    printf("Router %d listening for data on port %d\n",down.idNumber,down.port);
#endif
    
    char * selfstr = "";
    asprintf(&selfstr,"%s%s|",routerToString(routerOfIndex(self.idNumber, routers)),"");

    
    printf("IDENTIFICADOR: %s",selfstr);
    while(1)
    {
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(down.buf,'\0', MAX_USER_MSG_SIZE);
        
        //try to receive some data, this is a blocking call
        if ((down.recv_len = recvfrom(down.s, down.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &down.si_other, &down.slen)) == -1)
        {
            die("recvfrom()");
        }
        
        //now reply the client with the same data
        if (sendto(down.s, down.buf, down.recv_len, 0, (struct sockaddr*) &down.si_other, down.slen) == -1)
        {
            die("sendto()");
        }
        
        char *routedMessage = replace(down.buf, routerToString(routerOfIndex(down.idNumber, routers)), "");
        char** tokens;
        tokens = str_split(routedMessage, '|');
        
        routedMessage = replace(down.buf, routerToString(routerOfIndex(down.idNumber, routers)), "");

        if (!tokens[2]) {
            printf("\n\nMensagem recebida de %s", replace(routedMessage, "~|", " Conteúdo: "));
        }else{
            char * forward = replace(down.buf, selfstr, "");
            printf("\n\nEncaminhando pacote %s", forward);
            sendPackage(forward);
        }
    }
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

router chooseDestination(SelfRouter self_router, struct router routers[MAX_ROUTERS],struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS]){
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

void chat(struct router destination_router,struct linkr router_path,SelfRouter self_router,struct router routers[MAX_ROUTERS]){

    RouterUp request;
    char message[MAX_USER_MSG_SIZE];
    request= upRequest(destination_router, message);
    request=initUpClient(request);
    char header[MAX_HEADER_SIZE];
    char *content;
    
    strcpy(header, getHeader(router_path, routers));

    sleep(1);
    while (1) {
        
        printf("\n( :q to exit) Your message: ");
        scanf("%s",message);
        if (strcmp(":q", message)==0) {
            break;
        }
        asprintf(&content,"%s%s",header,message);
        
        strcpy(request.message, content);
        sendPackage(request.message);
    }
}


void interface(struct router routers[MAX_ROUTERS],struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS],SelfRouter self_router){

        linkr l;
        router r;
        r = chooseDestination(self_router, routers, linkGraph);
        l = linkGraph[self_router.idNumber][r.id];
        if (linkGraph[self_router.idNumber][r.id].cost<1) {
            printf("Destino não pode ser alcançado");
        }else{
            chat(r, l, self_router,routers);
        }
}


RouterUp upRequest(router destination, char message[MAX_USER_MSG_SIZE]){
    RouterUp request;
    strcpy(request.destination_IP, destination.ip);
    request.requestId = destination.id;
    request.port = destination.port;
    strcpy(request.message, message);
    return request;
}


char * getHeader(linkr l,struct router routers[MAX_ROUTERS]){
    char *line="";
    
    char** tokens;
    
    char *path_to_split[MAX_ROUTERS];
    strcpy((char*)path_to_split, l.path);
    tokens = str_split((char*)path_to_split, '-');
    
    for (int x = 0; x<=l.nodes ; x++) {
        router r = routerOfIndex(atoi(tokens[x]), routers);
        if (!x) {
            asprintf(&line,"%s%s~",line,routerToString(r));
        }else{
            asprintf(&line,"%s%s|",line,routerToString(r));
        }
    }
    
    return line;
}

Package packageFromString(char *s){
    
    char separator = '@';
    char** tokens;
    tokens = str_split(s, separator);
    
    Package p;
    
    p.localId = atoi(tokens[0]);
    p.destinationId = atoi(tokens[1]);
    strcpy(p.destinationIP, tokens[2]);
    p.ttl = atoi(tokens[3]);
    p.type = atoi(tokens[4]);
    p.senderId = atoi(tokens[5]);
    strcpy(p.senderIP, tokens[6]);
    strcpy(p.message, tokens[7]);
    p.status = atoi(tokens[8]);
    
    return p;
}


char * stringFromPackage(Package p){
    char *str;
    char separator = '@';
    asprintf(&str, "%d%c%d%c%s%c%d%c%d%c%d%c%s%c%s",
            p.localId,separator,
            p.destinationId,separator,
            p.destinationIP,separator,
            p.ttl,separator,
            p.type,separator,
            p.senderId,separator,
            p.senderIP,separator,
            p.message);

    return str;
}


//========================================
#pragma mark - MAIN
//========================================


int main(int argc, const char * argv[]) {

    printf("%s", argv[0]);
        
    if (argc<2) { // É NECESSÁRIO AO MENOS O ID DO ROTEADOR PARA FUNCIONAMENTO
            printf("\n Usage: ./main <router id>\n");
    }else{
    
        autoIncrementalLocalRequestId=0;
        
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
        
        
        //ROUTING PATHS
        add_links(linkCount, links,routerCount);

        
        Package p;
        p.localId = 111;
        strcpy(p.senderIP, "127.0.0.123");
        p.ttl = 255;
        p.type = 3;
        strcpy(p.destinationIP, "127.0.0.1");
        p.senderId = 1;
        strcpy(p.message, "MESSAGEEEEE");
        p.destinationId = 123;
        

        char *str = stringFromPackage(p);
        Package newpackage = packageFromString(str);

        
        
        struct linkr linkGraph[MAX_ROUTERS][MAX_ROUTERS];
        prepareRoutingPaths(linkGraph);
        
        if (strcmp(argv[2], "c")==0) {
            //SINGLETON FOR DOWNLOAD/LISTENING DATA
            prepareForDownload(self.download);
            
            sleep(1); //preparation for singleton init
            interface(routers, linkGraph, self);
            //interface(routers/*, links*/,self);
        }else if((int)argv[2] == 'r'){
            printf("STARTING ROUTING MODE ID: %s \n",argv[1]);
            self.download = initDownClient(self.download);

            routing(self, self.download, routers);
        }
//        
//        router self_router;
//        self_router.id = 1;
//        strcpy(self_router.ip, "127.0.0.1");
//        self_router.port = 8888;
//
//        SelfRouter self;
//        self.download.port = self_router.port;
//        self.idNumber = self.download.idNumber = self_router.id;
//        
//        prepareForDownload(self.download);
//
//        
//        SelfRouter selfi;
//        selfi.download.port = self_router.port;
//        selfi.idNumber = selfi.download.idNumber = self_router.id;
//        
//        prepareForDownload(selfi.download);

        
        
        
        
        
    
    }
    return 0;
}
