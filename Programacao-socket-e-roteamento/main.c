
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
Package sendingBuffer[ROUTING_BUFFER_SIZE];
Package receivingBuffer[ROUTING_BUFFER_SIZE];
int sendingBufferIndex, receivingBufferIndex;

struct connections conn;
int autoIncrementalLocalRequestId; //teletar

int stdOutDebugLevel;


void die(char *s)
{
    perror(s);
    exit(1);
}

int getRequestIdForPackage(){
    if (autoIncrementalLocalRequestId==INT_MAX-1000) {
        autoIncrementalLocalRequestId=0;
    }
    return autoIncrementalLocalRequestId++;
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

int indexOfLinkInConnections(linkr l, connections conn){
    for (int x =0; x<conn.linksCount; x++) {
        if (l.from == conn.linksList[x].from &&
            l.to == conn.linksList[x].to &&
            l.cost == conn.linksList[x].cost) {
            return x;
        }
    }
    return -1;
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

int addRouter(connections *conn, router r){
    conn->routerList[conn->routerCount]=r;
    conn->routerCount++;
    return 1;
}

int removeRouter(connections *conn, router r){
    int indx=-2;
    for (int x=0; x<conn->routerCount; x++) {
        if (conn->routerList[x].id==r.id) {
            indx=x;
        }
    }

    if (indx<0) {
        printf("Nó nao encontrado");
    }else if (indx>=0) {
        router newRouter;
        if (indx==conn->routerCount-1) {
            conn->routerList[indx]=newRouter;
            conn->routerCount--;
        }else{
            conn->routerList[indx]=conn->routerList[conn->routerCount-1];
            conn->routerList[conn->routerCount-1]=newRouter;
            conn->routerCount--;
        }
        return 1;
    }
    return 0;
}

int addLink(connections *conn, char linktxt[10]){
    int status = 0;
    linkr l;
    if (strcmp(linktxt, "")==0) {
        printf("\n\n=====[Inserir enlace]=====\nAdicione as informações do enlace no formato: origem-destino-custo:\n");
        char linktxta[10];
        scanf("%s",linktxta);
        l = linkFromChar(linktxta, "-");
    }else{
        l = linkFromChar(linktxt, "-");
    }
    if (indexOfLinkInConnections(l, *conn)<0) {
        if (routerOfIndex(l.to, conn->routerList).id<0) {
            router r;
            r.id = l.to;
            sprintf(r.ip, "%d",l.from);
            r.port = -1;
            addRouter(conn, r);

        }
        l.isDirectlyConnected=0;
        conn->linksList[conn->linksCount] = l;
        conn->linksCount++;
        return 1;
    }else{
        return 0;
    }
    return status;
}


int editLink(connections *conn, char linktxt[10]){
    int status = 0;
    int indx=-1;
    while (indx<0) {
        linkr l;
        if (strcmp(linktxt, "")==0) {
            printf("\n\n=====[Editar enlace]=====\n");
            printf("Adicione as informações do enlace à ser editado no formato: origem-destino-custo:\n(:menu para sair) Enlace: ");
            char linktxta[10];
            scanf("%s",linktxta);
            l = linkFromChar(linktxta, "-");
        }else{
            l = linkFromChar(linktxt, "-");
        }
        indx=indexOfLinkInConnections(l, *conn);
        if (indx>=0) {
            printf("\n\nInsira o novo custo para de %d para %d: ",l.from, l.to);
            scanf("%d",&conn->linksList[indx].cost);
            return 1;
        }else{
            return 0;
        }
        
    }
    
    return status;
}

int removeLink(connections *conn, char linktxt[10]){
    int status = 0;
    int indx=-1;
    while (indx<0) {
        linkr l;
        if (strcmp(linktxt, "")==0) {
            printf("\n\n=====[Remove enlace]=====\n");
            printf("Adicione as informações do ENLACE à ser REMOVIDO no formato: origem-destino-custo:\n(:menu para sair) Enlace: ");
            char linktxta[10];
            scanf("%s",linktxta);
            l = linkFromChar(linktxta, "-");
        }else{
            l = linkFromChar(linktxt, "-");
        }
        indx=indexOfLinkInConnections(l, *conn);
        if (indx>=0) {
            linkr newLink;
            if (indx==conn->linksCount-1) {
                conn->linksList[indx]=newLink;
                conn->linksCount--;
            }else{
                conn->linksList[indx]=conn->linksList[conn->linksCount-1];
                conn->linksList[conn->linksCount-1]=newLink;
                conn->linksCount--;
            }
            return 1;
        }else{
            return 0;
        }
        
    }
    return status;
}

void prepareRoutingTable(connections *conn){
    for (int x=0; x<MAX_LINKS; x++) {
        for (int y=0; y<MAX_LINKS; y++) {
            if (x==y) {
                conn->routingTable[x][y].cost=0;
            }else{
                conn->routingTable[x][y].cost=99;
            }
        }
    }
}

void updateRoutingTable(connections conn){
//    for (int x=0; x<conn.linksCount; x++) {
//        linkr l = conn.linksList[x];
//        if (conn.routingTable[l.from][l.to].cost>l.cost) {
//            conn.routingTable[l.from][l.to].cost
//        }
//    }
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


uploadSocket newSendRequestForPackage(Package p){

    uploadSocket newRequest;
    newRequest.port = p.port;
    newRequest.requestId = 0;
    if (p.status==PACKAGE_STATUS_DEAD) {
        return newRequest;
    }
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
//            die("socket");
            printf("SOCKET ERROR");
            newRequest.requestId=-1;
            break;
            if (stdOutDebugLevel>=DEBUG_REQUEST_FAILS)printf("inet_aton() failed creating request with port id %d",newRequest.port);
            
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
    
    while (sendTries-- && sendRequest.requestId && (sendRequest.package.status==PACKAGE_STATUS_PROCESSING||
                                                    sendRequest.package.status==PACKAGE_STATUS_SENDING||
                                                    sendRequest.package.status==PACKAGE_STATUS_READY)) {
        
        status = 0;
        if (sendto(sendRequest.s, messageString, strlen(messageString) , 0 , (struct sockaddr *) &sendRequest.si_other, sendRequest.slen)==-1)
        {
            //die("error sendto()");
            if (stdOutDebugLevel>=DEBUG_REQUEST_FAILS)printf("\Erro ao enviar mensagem para %d:%d",sendRequest.package.destinationId, sendRequest.port);
            return status;
        }else{
            status++;
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\n-> %s",stringFromPackage(sendRequest.package));
        }
        
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(sendRequest.buf,'\0', MAX_USER_MSG_SIZE);
        
        //try to receive some data, this is a blocking call
        if (recvfrom(sendRequest.s, sendRequest.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &sendRequest.si_other, &sendRequest.slen) == -1)
        {
            //die("recvfrom()");
            if (stdOutDebugLevel>=DEBUG_REQUEST_FAILS)printf("\n!-! FALHA na tentativa de receber ACK para o pacote %s",messageString);
        }else{
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\n<=> Confirmação de recebimento: %s",   sendRequest.buf);
            
            Package p = packageFromString(sendRequest.buf);
            if (!(p.type==PACKAGE_TYPE_BROADCAST_ACK)) {
                receivingBuffer[receivingBufferIndex]=p;
                receivingBuffer[receivingBufferIndex].status=PACKAGE_STATUS_READY;
                receivingBufferIndex++;
            }
            
            return ++status;
            break;
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

void addSendPackageToBuffer(Package p){
    sendingBuffer[sendingBufferIndex] = p;
    sendingBufferIndex++;
    if (sendingBufferIndex==ROUTING_BUFFER_SIZE) {
        sendingBufferIndex=0;
    }
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
//            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\nMENSAGEM RECEBIDA DE %s:%d\n", inet_ntoa(conn.downloadSocket.si_other.sin_addr), ntohs(conn.downloadSocket.si_other.sin_port));
            if (stdOutDebugLevel>=DEBUG_PACKAGE_ROUTING)printf("\n<- %s" , conn.downloadSocket.buf);
            
            //add to buffer
            if (receivingBufferIndex==ROUTING_BUFFER_SIZE && receivingBuffer[1].status==PACKAGE_STATUS_DONE) {
                receivingBufferIndex=1;
            }else if(receivingBufferIndex==ROUTING_BUFFER_SIZE && receivingBuffer[1].status!=PACKAGE_STATUS_DONE){
                printf("!!!!!!BUFFER DE RECEBIMENTO CHEIO!!!!!");
                exit(0);
            }
            Package received = packageFromString(conn.downloadSocket.buf);
            received.status = PACKAGE_STATUS_READY;
            if (receivingBufferIndex==ROUTING_BUFFER_SIZE) {
                receivingBufferIndex=1;
            }
            receivingBuffer[receivingBufferIndex]=received;
            receivingBufferIndex++;

            Package ackPkg = ackPackage(received);
            //now reply the client with the same data
            if (sendto(conn.downloadSocket.s, stringFromPackage(ackPkg), conn.downloadSocket.recv_len, 0, (struct sockaddr*) &conn.downloadSocket.si_other, conn.downloadSocket.slen) == -1)
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

int isBetterLink(linkr a,linkr b){
    if (a.cost<b.cost) {
        return 1;
    }else{
        return 0;
    }
}

void updateRoutingTableWithPackage(Package p){
    char *token;
    char *rest = p.message;
    
    while((token = strtok_r(rest, "|", &rest)))
    {
        char *packageToken;
        char *packagerest = token;
        
        char strLink[15];
        strcpy(strLink, token);

        linkr l;
        int i=0;
        while((packageToken = strtok_r(packagerest, "-", &packagerest))){
            switch (i) {
                case 0:
                    l.from=atoi(packageToken);
                    break;
                case 1:
                    l.to=atoi(packageToken);
                    break;
                case 2:
                    l.cost=atoi(packageToken);
                    break;
            }
            i++;
        }
        
        if (isBetterLink(l, conn.routingTable[l.from][l.to])&&
            isBetterLink(l, conn.routingTable[l.to][l.from])&&
            isBetterLink(l, conn.routingTable[conn.selfID][l.to])){
            if (addLink(&conn, strLink)) {
                printf("\n!!Novo enlace adicionado!! %d - %d - %d\n", l.from, l.to, l.cost);
                conn.routingTable[l.from][l.to]=l;
                conn.routingTable[l.to][l.from]=l;

            }
        }else{
         //   printf("Não houveram atualizações");
        }
    }
    //free(tokens);
    //criar o algoritmo de convergencia dos vetores de distancia
    
    
}
void setackPackage(Package p){
    printf("\n<=============================\n| Mensagem %d com sucesso!! \n<=============================\n", p.packageId);
    for (int x =0; x<receivingBufferIndex-1; x++) {
        if (p.packageId==receivingBuffer[x].packageId) {
            receivingBuffer[x].status = PACKAGE_STATUS_ACK;
        }
    }
}

void printForwardPackage(Package p){
    printf("\n<---------->\n| Pacote recebido de %d: IP: %s  PORTA: %d ", p.localId,p.destinationIP,p.port);
    printf("\n| Salva no buffer encaminhamento: %s \n<---------->\n", p.message);

}

void printMessagePackage(Package p){
    printf("\n<=============================\n| Mensagem recebida de %d: IP: %s  PORTA: %d ", p.localId,p.destinationIP,p.port);
    printf("\n| Conteudo: %s \n<=============================\n", p.message);

}

void * routing(){

    for (int pid=0; ; pid++) {
        
        Package p = receivingBuffer[pid];
        if (p.status==PACKAGE_STATUS_READY) {
            p.status=PACKAGE_STATUS_PROCESSING;
            switch (p.type) {
                case PACKAGE_TYPE_BROADCAST:
                    updateRoutingTableWithPackage(p);
                    break;
                case PACKAGE_TYPE_MESSAGE:
                    printMessagePackage(p);
                    break;
                case PACKAGE_TYPE_FORWARD:
                    printForwardPackage(p);
                    Package toRoute = packageFromString(p.message);
                    toRoute.packageId = getRequestIdForPackage();
                    addSendPackageToBuffer(toRoute);
                    break;
                case PACKAGE_TYPE_ACK:
                    setackPackage(p);
                break;

            }
            receivingBuffer[pid].status=PACKAGE_STATUS_DONE;

        }
        if (pid>=receivingBufferIndex-1) {
            pid=0;
            
        }
    }
    
    
}

Package ackPackage(Package p){
    
    //inverte a direção do pacote
    p.destinationId += p.localId;
    p.localId = p.destinationId-p.localId;
    p.destinationId = p.destinationId - p.localId;
 
    router r = routerOfIndex(p.destinationId, conn.routerList);

    strcpy(p.destinationIP, r.ip);
    p.port = r.port;
    if (p.type==PACKAGE_TYPE_BROADCAST) {
        p.type=PACKAGE_TYPE_BROADCAST_ACK;
    }else{
        p.type = PACKAGE_TYPE_ACK;
    }
    strcpy(p.message, "");
    p.status = PACKAGE_STATUS_READY;
    
    return p;
}


Package packageFromString(char *s){
    
//    char separator = '@';
//    char** tokens;
//    tokens = str_split(s, separator);
//    
//    Package p;
//    
//    p.localId = atoi(tokens[0]);
//    p.destinationId = atoi(tokens[1]);
//    strcpy(p.destinationIP, tokens[2]);
//    p.port = atoi(tokens[3]);
//    p.ttl = atoi(tokens[4]);
//    p.type = atoi(tokens[5]);
////    p.senderId = atoi(tokens[6]); //teletar
//    strcpy(p.senderIP, tokens[6]);
//    strcpy(p.message, tokens[7]);
//    p.status = PACKAGE_STATUS_READY;

    
    char *token;
    char *rest = s;
    
    Package p;
    int i=0;
    while((token = strtok_r(rest, "@", &rest)))
    {
        
        //    p.senderId = atoi(tokens[6]); //teletar

        switch (i) {
            case 0:
                p.localId = atoi(token);
                break;
            case 1:
                p.destinationId = atoi(token);
                break;
            case 2:
                p.packageId = atoi(token);
                break;
            case 3:
                strcpy(p.destinationIP, token);
                break;
            case 4:
                p.port = atoi(token);
                break;
            case 5:
                p.ttl = atoi(token);
                break;
            case 6:
                p.type = atoi(token);
                break;
            case 7:
                strcpy(p.senderIP, token);
                if (p.type==PACKAGE_TYPE_FORWARD) {
                    strcpy(p.message, rest);
                }
                break;
            case 8:
                strcpy(p.message, token);
                break;
        }
        if (i==7 && p.type==PACKAGE_TYPE_FORWARD) break;
        i++;
        
    }
    p.status = PACKAGE_STATUS_READY;

    
    return p;
}


char * stringFromPackage(Package p){
    char *str;
    char separator = '@';
    asprintf(&str, "%d%c%d%c%d%c%s%c%d%c%d%c%d%c%s%c%s",
             p.localId,separator,
             p.destinationId,separator,
             p.packageId,separator,
             p.destinationIP,separator,
             p.port,separator,
             p.ttl,separator,
             p.type,separator,
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

char * getLinkStringToBroadCast(connections conn, linkr l){
    char *str = "";
    int y=conn.linksCount-1;
    asprintf(&str, "%s%d-%d-%d|",str,l.to,l.from,l.cost);
    while (y>=0) {
        if (l.to!=conn.linksList[y].from && l.to!=conn.linksList[y].to) {
            asprintf(&str, "%s%d-%d-%d|",str,conn.linksList[y].from,conn.linksList[y].to,conn.linksList[y].cost+l.cost);
        }
        y--;
    }

    return str;
}

void * sendLinksBroadcast(){
    int x=0;
    while (1) {
        if (x<0) x=0;
        if (conn.linksCount>0 && conn.linksList[x].isDirectlyConnected==1) {
            Package p;
            p.localId = conn.selfID;
            p.destinationId = conn.linksList[x].to;
            p.packageId = getRequestIdForPackage();
            strcpy(p.destinationIP, routerOfIndex(conn.linksList[x].to, conn.routerList).ip);
            p.ttl=MAX_TTL_BROADCAST;
            p.type=PACKAGE_TYPE_BROADCAST;
            strcpy(p.senderIP, conn.routerList[x].ip);
            strcpy(p.message, getLinkStringToBroadCast(conn,conn.linksList[x]));
            p.status=PACKAGE_STATUS_READY;
            p.port = routerOfIndex(conn.linksList[x].to, conn.routerList).port;
            
//            uploadSocket sendRequest = newSendRequestForPackage(p);
            addSendPackageToBuffer(p);
//            while (sendRequest.requestId>0 && sendRequest.package.ttl>=0) {
//                if (sendRequest.package.ttl<=0) {
////                    removeAllId(sendRequest.package.destinationId);
//                    break;
//                }else if (sendPackageWithRequest(sendRequest)==REQUEST_STATUS_SEND_NO_ANSWER) {
//                    sendRequest.package.ttl--;
//                }
//            }
        }
        if (x>=conn.linksCount-1) {
            sleep(5);
            x=0;
        }else{
            x++;
        }


    }
    return NULL;
}

linkr connectedLinkToDestinationId(int from, int to){
    linkr shortestNext;
    shortestNext.cost=999;
    for (int r=0; r<conn.linksCount; r++) {
        if ((conn.linksList[r].to==to||conn.linksList[r].from==to)&&conn.linksList[r].cost<shortestNext.cost) {
            shortestNext=conn.linksList[r];
        }
    }
    if (/*shortestNext.from==from||shortestNext.to==to*/shortestNext.isDirectlyConnected) {
        return shortestNext;
    }else{
        return connectedLinkToDestinationId(from, shortestNext.from);
    }
}

int indexForBidirectionalLink(linkr l){
    router r1 = routerOfIndex(l.from, conn.routerList);
    router r2 = routerOfIndex(l.to, conn.routerList);
    if (r1.port>0 && r2.port<0) {
        return r1.id;
    }else if (r2.port>0 && r1.port<0){
        return r2.id;
    }else if(r1.id!=conn.selfID && r2.id==conn.selfID){
        return r1.id;
    }else if(r2.id!=conn.selfID && r1.id==conn.selfID){
        return r2.id;
    }else{
        return -1;
    }
}

Package routedPackage(Package p){
    linkr l = connectedLinkToDestinationId(p.localId, p.destinationId);
    if (l.isDirectlyConnected==1&&(p.destinationId==l.to||p.destinationId==l.from)) { //teletar
//        printf("Directly connected"); //teletar
        return p;
    }else{
        router r = routerOfIndex(indexForBidirectionalLink(l), conn.routerList);
        Package routedPackage;
        
        if (r.id<0) {
            routedPackage.status=PACKAGE_STATUS_DEAD;
        }else{
            routedPackage.status=PACKAGE_STATUS_READY;
            routedPackage.localId = conn.selfID ;
            routedPackage.destinationId = l.to;
            routedPackage.packageId = getRequestIdForPackage();
            strcpy(routedPackage.destinationIP, r.ip);
            routedPackage.ttl=MAX_TTL;
            routedPackage.type=PACKAGE_TYPE_FORWARD;
            strcpy(routedPackage.senderIP, conn.selfRouter.ip);
            strcpy(routedPackage.message, stringFromPackage(p));
            routedPackage.port = r.port;
        }
        
        return routedPackage;
    }
}

void * flushSendBuffer(){
    while (1) {
//        sleep(1);
        
        for (int index = 0; sendingBufferIndex; index++) {
            if (index>sendingBufferIndex) {
                index=0;
            }
            if (sendingBuffer[index].status==PACKAGE_STATUS_SENT) {
                sendingBuffer[index].status=PACKAGE_STATUS_READY;
                sendingBuffer[index].ttl--;
            }
            if (sendingBuffer[index].ttl<0 && (sendingBuffer[index].status==PACKAGE_STATUS_READY || sendingBuffer[index].status==PACKAGE_STATUS_SENT)) {
                sendingBuffer[index].status=PACKAGE_STATUS_DEAD;
                printf("!! NAO É POSSIVEL ENVIAR PARA ESTE DESTINO %s",stringFromPackage(sendingBuffer[index]));
                removeAllId(sendingBuffer[index].destinationId);
            }
            Package p = sendingBuffer[index];

            if (p.status==PACKAGE_STATUS_READY) {
                sendingBuffer[index].status = PACKAGE_STATUS_SENDING;
                    uploadSocket sendRequest = newSendRequestForPackage(routedPackage(p));
                int status;
                if (sendRequest.requestId>0) {
                    status = sendPackageWithRequest(sendRequest);
                }else{
                    sendingBuffer[index].status=PACKAGE_STATUS_DEAD;
                    break;
                }
                switch (status) {
                    case REQUEST_STATUS_ERROR:
                        printf("!! ERRO AO ENVIAR PACOTE %s",stringFromPackage(sendRequest.package));
                        exit(0);
                        break;
                    case REQUEST_STATUS_SEND_NO_ANSWER:
                        sendingBuffer[index].ttl--;
                        sendingBuffer[index].status= PACKAGE_STATUS_READY;
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

    char message[MAX_USER_MSG_SIZE];
    
    sleep(1);
    while (1) {
        int msgid=getRequestIdForPackage();
        
        printf("\n( :menu to exit) Insira o conteúdo da mensagem id %d para roteador id %d: ", msgid, destinationRouter.id);
        scanf("%s",message);
        if (strcmp(":menu", message)==0) {
            break;
        }
        
        Package p;
        p.localId = conn.selfID;
        p.destinationId = destinationRouter.id;
        p.packageId =msgid;
        strcpy(p.destinationIP, destinationRouter.ip);
        p.ttl=MAX_TTL;
        p.type=PACKAGE_TYPE_MESSAGE;
        strcpy(p.senderIP, destinationRouter.ip);
        p.status=PACKAGE_STATUS_READY;
        strcpy(p.message, message);
        p.port = destinationRouter.port;
        addSendPackageToBuffer(p);

        
        
    }
}

void removeAllId(int idx){
    for (int x=0; x<conn.linksCount; x++) {
        if (conn.linksList[x].to==idx || conn.linksList[x].from==idx) {
            char *linktxt;
            asprintf(&linktxt, "%d-%d-%d",conn.linksList[x].from,conn.linksList[x].to,conn.linksList[x].cost);
            removeLink(&conn, linktxt);
        }
    }
    for (int x=0; x<conn.routerCount; x++) {
        if (conn.routerList[x].id==idx) {
            removeRouter(&conn, conn.routerList[x]);
        }
    }
}

void presentRoutingTable(connections conn){
    printf("\n=====[TABELA DE ROTEAMENTO]=====\n\n   ");
    for (int y=0; y<conn.routerCount; y++) {
        printf(" %2d ",conn.routerList[y].id);
    }
    printf("\n--+");
    for (int y=0; y<conn.routerCount; y++) {
        printf("----");
    }

    for (int x=0; x<MAX_LINKS; x++) {
        if (conn.routerList[x].id>0) {
            printf("\n%2d|",conn.routerList[x].id);
            for (int y=0; y<conn.routerCount; y++) {
                    printf(" %2d ",conn.routingTable[conn.routerList[x].id][conn.routerList[y].id].cost);
            }
        }
        
    }
        
}

void interface(connections *conn){
    char option[10] = ":menu";
    while (1) {

            if (strcmp(option, "1")==0) {
                println(5);
                printf("\n\n=====[Tabela de roteamento]=====\n");
                printLinks(*conn);
                printf("=====[Detalhamento dos nos encontrados]=====\n");
                printRouters(*conn);
                printf("\n\n [ 1 ] Adicionar enlace\n [ 2 ] Editar enlace\n [ 3 ] Remover enlace \n Selecione um item (:menu para voltar):");
                scanf("%9s",option);
                if (strcmp(option, "1")==0) {
                    if (addLink(conn, "")) {
                        printf("\n\nEnlace adicionado!!\n\n");
                    }else{
                        printf("\n\nEnlace já existente!!\n\n");
                    }
                    strcpy(option, ":menu");
                }else if (strcmp(option, "2")==0) {
                    editLink(conn, "");
                    strcpy(option, ":menu");
                }else if (strcmp(option, "3")==0) {
                    
                    if(removeLink(conn, "")){
                        printf("!!Enlace removido!!");
                    }else{
                        printf("!!Enlace não removido!!");
                    }
                    strcpy(option, ":menu");
   
                }else{
                    
                }
                
            }
            if (strcmp(option, "2")==0) {
                println(5);
                printf("\n\n=====[Lista de enlaces]=====\n");
                printLinks(*conn);
                printf("\n\n=====[Detalhamento dos enlaces]=====\n");
                printRouters(*conn);
                router r;
//                while (r.id<0) {
                    r = chooseDestination(*conn);
//                }
                chat(r, *conn);

            }
            if (strcmp(option, "3")==0) {
                char package[MAX_PACKAGE_SIZE];
                printf("\n Insira o pacote a enviar:");
                scanf("%s",package);
                Package new = packageFromString((char*)package);
                new.status=PACKAGE_STATUS_READY;
                addSendPackageToBuffer(new);
            }
            if (strcmp(option, "4")==0) {
                presentRoutingTable(*conn);
            }
//            if (strcmp(option, ":menu")==0) {
            println(5);
            printf("\n\n==================[ROTEADOR SOCKET UPD]==================\n");
            printf("[ 1 ] Enlaces e nos conectados\n[ 2 ] Enviar mensagem para cliente\n[ 3 ] Enviar pacote\n");
            printf("[ 4 ] Tabela de roteamento\nSelecione um item: ");
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
        sendingBufferIndex=receivingBufferIndex=0;
        
        stdOutDebugLevel = DEBUG_ALL;

        prepareRoutingTable(&conn);
        
        //CONFIG FILE READING
        conn = readLinks(PATH_LINKS_FILE, conn);
        conn = readRouters(PATH_ROUTER_FILE, conn);
        addRouter(&conn, conn.selfRouter);
        
        
        pthread_t flushSendBufferSingleton;
        pthread_create(&flushSendBufferSingleton, NULL, flushSendBuffer, NULL);
      
        pthread_t sendLinksBroadcastSingleton;
        pthread_create(&sendLinksBroadcastSingleton, NULL, sendLinksBroadcast, NULL);
        
        pthread_t downloadSocketSingleton = initDownloadSocketThread(&conn);

        pthread_t routingSingleton;
        pthread_create(&routingSingleton, NULL, routing, NULL);

        
        sleep(1); // tempo para o socket de download ser inicializado
        interface(&conn);
        
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
