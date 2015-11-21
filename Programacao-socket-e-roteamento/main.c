
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
Package sendingBuffer[SENDING_BUFFER_SIZE];
Package receivingBuffer[SENDING_BUFFER_SIZE];
int sendingBufferIndex, receivingBufferIndex;

struct connections conn;
int autoIncrementalLocalRequestId; //teletar

int stdOutDebugLevel;


void die(char *s)
{
    perror(s);
    exit(1);
}

router routerOfIndex(int indx, router r[conn.routerCount]){ //teletar?
    for (int x=0; x<conn.routerCount; x++) {
        if (r[x].id==indx) {
            return r[x];
        }
    }
    router null;
    null.id=-2;
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

uploadSocket initUpClient(uploadSocket up){ //teletar

    
    
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


uploadSocket newSendRequestForPackage(Package p, int portNumber){

    uploadSocket newRequest;
    newRequest.port = portNumber;
    newRequest.requestId = 0;
    strcpy(newRequest.destination_IP, p.destinationIP);
    
    while (!newRequest.requestId) {
        newRequest.slen = sizeof(newRequest.si_other);
        newRequest.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (newRequest.s == -1){
            die("socket");
        }
        
        memset((char *) &newRequest.si_other, 0, sizeof(newRequest.si_other));
        newRequest.si_other.sin_family = AF_INET;
        newRequest.si_other.sin_port = htons(newRequest.port);
        
        if (inet_aton(newRequest.destination_IP , &newRequest.si_other.sin_addr) == 0)
        {
            die("socket");

            if (stdOutDebugLevel>=DEBUG_REQUEST_FAILS)printf("inet_aton() failed creating request with port id %d",newRequest.port);
            
            newRequest.port++;
        }else{
            newRequest.package = p;
            newRequest.requestId = p.localId;
        
            struct timeval tv;
            tv.tv_sec       = 1;
            tv.tv_usec      = 1000;
            
            if (setsockopt(newRequest.s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
                perror("send Message Socket Error");
            }
        }
        
        

    }
    
    return newRequest;
}
int sendPackageWithRequest(uploadSocket sendRequest){
    
    int sendTries = SEND_TRIES;
    if (sendRequest.package.type==PACKAGE_TYPE_BROADCAST) {
        sendTries=1;
    }
    
    int status = 0;
    
    char *messageString = stringFromPackage(sendRequest.package);
    
    while (sendTries--) {
        

        status = 0;
        if (sendto(sendRequest.s, messageString, strlen(messageString) , 0 , (struct sockaddr *) &sendRequest.si_other, sendRequest.slen)==-1)
        {
            //die("error sendto()");
            if (stdOutDebugLevel>=DEBUG_REQUEST_FAILS)printf("\Erro ao enviar mensagem %d para %s:%d",sendRequest.requestId,sendRequest.destination_IP, sendRequest.port);
            return status;
        }else{
            status++;
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\nMensagem id %d enviada para %s:%d",sendRequest.requestId,sendRequest.destination_IP, sendRequest.port);
        }
        
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(sendRequest.buf,'\0', MAX_USER_MSG_SIZE);
        
        //try to receive some data, this is a blocking call
        if (recvfrom(sendRequest.s, sendRequest.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &sendRequest.si_other, &sendRequest.slen) == -1)
        {
    #ifdef DEBUG_LEVEL_3
            //die("recvfrom()");
            if (stdOutDebugLevel>=DEBUG_REQUEST_FAILS)printf("\nFALHA na tentativa de receber ACK %s:%d",sendRequest.destination_IP, sendRequest.port);
            
    #endif
        }else{
    #ifdef DEBUG_LEVEL_3
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\n\nConfirmação de recebimento: %s:%d",sendRequest.destination_IP, sendRequest.port);
            return ++status;
            break;
    #endif
        }
    }
    return status;
}

void closeUp(uploadSocket up){
    close(up.s);
#ifdef DEBUG_LEVEL_3
    printf("Up Client Closed\n");
#endif
}

void sendPackage(char *s){//teletar
//    RouterUp request;
//    char string[MAX_HEADER_SIZE];
//    
//    strcpy(string, s);
//    char **a = str_split(string, '~');
//    char **b = str_split(a[1], '|');
//
//    router r = stringToRouter(b[0]);
//    request= upRequest(r, s);
//    request=initUpClient(request);
//    sendMessage(request);
}



void addSendPackageToBuffer(Package p){
    sendingBuffer[sendingBufferIndex] = p;
    sendingBufferIndex++;
}








//======================================================
#pragma mark - DOWNLOAD
//======================================================

pthread_t initDownloadSocketThread(connections *conn){
    conn->downloadSocket.port = conn->selfRouter.port;
    conn->downloadSocket.idNumber = conn->selfRouter.id;
    conn->downloadSocket = initDownClient(conn->downloadSocket);
    pthread_t download_Singleton;
    
    pthread_create(&download_Singleton, NULL, startDownListen, NULL);
    
    //    startDownListen(down);
    return download_Singleton;
}

downloadSocket initDownClient(downloadSocket down){
    down = down;
    
    down.slen = sizeof(down.si_other);
    
    //create a UDP socket
    if ((down.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    // zero out the structure
    memset((char *) &down.si_me, 0, sizeof(down.si_me));
    
    down.si_me.sin_family = AF_INET;
    down.si_me.sin_port = htons(down.port);
    down.si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(down.s , (struct sockaddr*)&down.si_me, sizeof(down.si_me) ) == -1)
    {
        die("bind");
    }
    return down;
}


void * startDownListen(void){ //listener to download data on thread
    
    //keep listening for data
    printf("Router %d listening for data on port %d\n",conn.downloadSocket.idNumber,conn.downloadSocket.port);
    while(1)
    {
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(conn.downloadSocket.buf,'\0', MAX_USER_MSG_SIZE);
        
        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        if (setsockopt(conn.downloadSocket.s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            perror("Error");
        }
        
        
        //try to receive some data, this is a blocking call
        if ((conn.downloadSocket.recv_len = recvfrom(conn.downloadSocket.s, conn.downloadSocket.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &conn.downloadSocket.si_other, &conn.downloadSocket.slen)) == -1)
        {
//            printf("\nerro recvfrom() timeout\n");
        }else{
            //print details of the client/peer and the data received
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\nMENSAGEM RECEBIDA DE %s:%d\n", inet_ntoa(conn.downloadSocket.si_other.sin_addr), ntohs(conn.downloadSocket.si_other.sin_port));
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\nConteúdo: %s\n" , conn.downloadSocket.buf);
            
            //add to buffer
            receivingBuffer[receivingBufferIndex]=packageFromString(conn.downloadSocket.buf);
            receivingBufferIndex++;
            
            //now reply the client with the same data
            if (sendto(conn.downloadSocket.s, conn.downloadSocket.buf, conn.downloadSocket.recv_len, 0, (struct sockaddr*) &conn.downloadSocket.si_other, conn.downloadSocket.slen) == -1)
            {
                die("sendto()");
            }
            
        }
        
        
    }
}


void closeDown(downloadSocket down){
    close(down.s);
    
#ifdef DEBUG_LEVEL_3
    printf("Down Client Closed\n");
#endif
}




//========================================
#pragma mark - PACKAGE ROUTING
//========================================


void * routing(){
    while (1) {
        if (receivingBufferIndex>0) {
            int indx = receivingBufferIndex;
            while (indx) {
                Package p = receivingBuffer[receivingBufferIndex];
                switch (p.type) {
                    case PACKAGE_TYPE_BROADCAST:
                        printLinks(conn);
                        printf("%s",p.message);
                        break;
                    case PACKAGE_TYPE_MESSAGE:
                        
                        break;
                    default:
//                        indx--;
                    break;
                
            }
            }
        }else{
            sleep(1);
        }
    }
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
//    p.status = atoi(tokens[8]);
    
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

char * getBroadcastAdd(char *ip, int maskLevel){
    char *str = NULL;
    char** tokens;
    char *toSplit[MAX_PACKAGE_SIZE];
    strcpy((char*)toSplit, ip);
    tokens = str_split((char*)toSplit, '.');
    switch (maskLevel) {
        case 1:
            asprintf(&str, "%s.255.255.255",tokens[0]);
            break;
        case 2:
            asprintf(&str, "%s.%s.255.255",tokens[0],tokens[1]);
            break;
        case 3:
            asprintf(&str, "%s.%s.%s.255",tokens[0],tokens[1],tokens[2]);
            break;
        case 4:
            asprintf(&str, "%s.%s.%s.%s",tokens[0],tokens[1],tokens[2],tokens[3]);
            break;
    }
    return str;
}

char * getLinkStringToBroadCast(connections conn){
    char *str = "";
    int x=conn.linksCount-1;
    while (x>=0) {
        asprintf(&str, "%s%d-%d-%d|",str,conn.linksList[x].from,conn.linksList[x].to,conn.linksList[x].cost);
        x--;
    }

    return str;
}

void * sendLinksBroadcast(){
    int x=0;
    while (1) {
        
        Package p;
        p.localId = conn.selfID;
        p.destinationId = conn.routerList[x].id;
        strcpy(p.destinationIP, getBroadcastAdd(conn.routerList[x].ip, 4));
        p.ttl=255;
        p.type=PACKAGE_TYPE_BROADCAST;
        p.senderId=conn.routerList[x].id;
        strcpy(p.senderIP, conn.routerList[x].ip);
        strcpy(p.message, getLinkStringToBroadCast(conn));
        p.status=PACKAGE_STATUS_READY;

        uploadSocket sendRequest = newSendRequestForPackage(p, conn.routerList[x].port);
        sendPackageWithRequest(sendRequest);
        if (x==conn.routerCount-1) {
            sleep(10);
            x=0;
        }else{
            x++;
        }

    }
    return NULL;
}

void * flushSendBuffer(){
    while (1) {
//        sleep(1);
        
        for (int index = 0; sendingBufferIndex; index++) {
            if (index>sendingBufferIndex) {
                index=0;
            }
            Package p = sendingBuffer[index];
            if (p.status==PACKAGE_STATUS_READY) {
                sendingBuffer[index].status = PACKAGE_STATUS_SENDING;
                    uploadSocket sendRequest = newSendRequestForPackage(p,8888);
                    int status = sendPackageWithRequest(sendRequest);
                switch (status) {
                    case REQUEST_STATUS_ERROR:
                        
                        break;
                    case REQUEST_STATUS_SEND_NO_ANSWER:
                        
                        break;
                    case REQUEST_STATUS_OK:
                        sendingBuffer[index].status= PACKAGE_STATUS_SENT;
                        break;
                }
            }
        }
    }
    
}


//========================================
#pragma mark - VISUAL INTERFACE
//========================================

void println(int x){
    for (int y=0; y<=x; y++) {
        printf("\n");
    }
}

void printRouter(router r){
    printf("| ID: %d | Porta: %d | IP: %s\n",r.id,r.port,r.ip);
}

void printRouters(connections conn){
    for (int x=0; x<conn.routerCount; x++) { //teletar
        printRouter(conn.routerList[x]);
    }
}

void printlink(linkr l){
    printf("| FROM: %d | TO: %d | COST: %d\n",l.from,l.to,l.cost);
}
void printLinks(connections conn){
    for (int x=0; x<conn.linksCount; x++) {
        printlink(conn.linksList[x]);
    }
}

router chooseDestination(connections conn){
    printRouters(conn);
    router r;
    r.id=-1;
    while (r.id<0) {
        
        printf("\nEscolha o id do destinatário: ");
        int indx;
        scanf("%d",&indx);
        if (indx>0) {
            r=routerOfIndex(indx, conn.routerList);
        }
    }
    return r;
}

void chat(struct router destinationRouter, connections conn){

    uploadSocket request;
    char message[MAX_USER_MSG_SIZE];
    
    sleep(1);
    while (1) {
    
        Package p;
        p.localId = conn.selfID;
        p.destinationId = destinationRouter.id;
        strcpy(p.destinationIP, destinationRouter.ip);
        p.ttl=255;
        p.type=PACKAGE_TYPE_MESSAGE;
        p.senderId=conn.selfRouter.id;
        strcpy(p.senderIP, destinationRouter.ip);
        p.status=PACKAGE_STATUS_READY;
        strcpy(p.message, getLinkStringToBroadCast(conn));
        
    
        request= upRequest(destinationRouter, message);
        request=initUpClient(request);
        char header[MAX_HEADER_SIZE];
        char *content;
        
//        strcpy(header, getHeader(router_path, routers)); //header
        
        printf("\n( :menu to exit) Your message: ");
        scanf("%s",message);
        if (strcmp(":menu", message)==0) {
            break;
        }
        asprintf(&content,"%s%s",header,message);
        
        strcpy(request.message, content);
        sendPackage(request.message);
    }
}


void interface(connections conn){
    char option[10] = ":menu";
    while (1) {

            if (strcmp(option, "1")==0) {
                println(5);
                printf("\n\n=====[Tabela de roteamento]=====\n");
                printLinks(conn);
                printf("\n\n=====[Detalhamento dos nos encontrados]=====\n");
                printRouters(conn);
            }
            if (strcmp(option, "2")==0) {
                println(5);
                printf("\n\n=====[Tabela de roteamento]=====\n");
                printLinks(conn);
                printf("\n\n=====[Detalhamento dos nos encontrados]=====\n");
                printRouters(conn);
                router r;
//                while (r.id<0) {
                    r = chooseDestination(conn);
//                }
                chat(r, conn);

            }
            if (strcmp(option, "3")==0) {
                char package[MAX_PACKAGE_SIZE];
                printf("\n Insira o pacote a enviar:");
                scanf("%s",package);
                Package new = packageFromString((char*)package);
                new.status=PACKAGE_STATUS_READY;
                addSendPackageToBuffer(new);
            }
//            if (strcmp(option, ":menu")==0) {
            println(5);
            printf("\n\n==================[ROTEADOR SOCKET UPD]==================\n");
            printf("[ 1 ] Ver tabela de roteamento\n[ 2 ] Enviar mensagem para cliente\n[ 3 ] Enviar pacote\n Selecione um item: ");
//            }
        
        scanf("%9s",option);

        //        scanf("%s",package);

        
        
        
    }
    
}


uploadSocket upRequest(router destination, char message[MAX_USER_MSG_SIZE]){
    uploadSocket request;
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

//========================================
#pragma mark - MAIN
//========================================


int main(int argc, const char * argv[]) {

    printf("%s", argv[0]);
        
    if (argc<2) { // É NECESSÁRIO AO MENOS O ID DO ROTEADOR PARA FUNCIONAMENTO
            printf("\n Usage: ./main <router id>\n");
    }else{

        conn.selfID = atoi(argv[1]);

        autoIncrementalLocalRequestId=0; //teletar?
        
        stdOutDebugLevel = DEBUG_ALL;
        
        //CONFIG FILE READING
        conn = readLinks(PATH_LINKS_FILE, conn);
        conn = readRouters(PATH_ROUTER_FILE, conn);
        
        pthread_t flushSendBufferSingleton;
        pthread_create(&flushSendBufferSingleton, NULL, flushSendBuffer, NULL);
      
        pthread_t sendLinksBroadcastSingleton;
        pthread_create(&sendLinksBroadcastSingleton, NULL, sendLinksBroadcast, NULL);
        
        pthread_t downloadSocketSingleton = initDownloadSocketThread(&conn);

        pthread_t routingSingleton;
        pthread_create(&routingSingleton, NULL, routing, NULL);

        
        sleep(1); // tempo para o socket de download ser inicializado
        interface(conn);
        
        //GATEWAY CONFIG INITIALIZATION
//        router self_router = routerOfIndex(conn.selfID, conn.routerList);

        
        //ROUTING SINGLETONS
//        if (strcmp(argv[2], "c")==0) {
//            
//            //SINGLETON FOR DOWNLOAD/LISTENING DATA
//            prepareForDownload();
//            
//            sleep(1); //preparation for singleton init
//            interface(conn);
//            //interface(routers/*, links*/,self);
//        }else if(strcmp(argv[2], "r")==0){
//            printf("STARTING ROUTING MODE ID: %s \n",argv[1]);
//            self.download = initDownClient(self.download);
//
//            routing(self, self.download, conn.routerList);
//        }
        
        
        
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

        
//        Edge Arestas[10] = {{0,1,1},{1,2,2},{2,3,3}};
        // BellmanFord(Estrutura, arestas, vertices,origem);
//        BellmanFord(Arestas, 2, 3, 1);
    }
    return 0;
}
