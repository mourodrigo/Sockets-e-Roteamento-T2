
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
struct debug_std std;
int autoIncrementalLocalRequestId;

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

router routerOfIndex(int indx, router r[conn.routerCount]){
    for (int x=0; x<conn.routerCount; x++) {
        if (r[x].id==indx) {
            return r[x];
        }
    }
    router null;
    null.id=-2;
    return null;
}

int indexOfLinkInConnections(linkr l){
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
    for (int x=0; x<conn->routerCount; x++) {
        if (conn->routerList[x].id==r.id) {
            return 0;
        }
    }
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
        if(std.DEBUG_STD_ROUTER_ADD_REMOVE) printf("Nó %d nao encontrado para remoção",indx);
    }else if (indx>=0 && r.id!=conn->selfID) {
        router newRouter;
        newRouter.id=0;
        newRouter.port=-2;
        strcpy(newRouter.ip, "");
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

int addLink(linkr link){
    int status = 0;
    linkr l;
    if ((link.from<=0&&link.to<=0)) {
        printf("\n\n=====[Inserir enlace]=====\nAdicione as informações do enlace no formato: origem-destino-custo:\n");
        char linktxta[10];
        scanf("%s",linktxta);
        l = linkFromChar(linktxta, "-");
    }else{
        l = link;
    }
    if (indexOfLinkInConnections(l)<0) {
        l.ttl = MAX_LINK_TTL;
        conn.linksList[conn.linksCount] = l;
        conn.linksCount++;
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
        indx=indexOfLinkInConnections(l);
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
        indx=indexOfLinkInConnections(l);
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

//======================================================
#pragma mark - UPLOAD
//======================================================

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
            if(std.DEBUG_STD_FATAL_ERRORS) printf("\nAguardando socket livre para envio...");
            sleep(1);
        }else{
            
            memset((char *) &newRequest.si_other, 0, sizeof(newRequest.si_other));
            newRequest.si_other.sin_family = AF_INET;
            newRequest.si_other.sin_port = htons(newRequest.port);
            
            if (inet_aton(newRequest.destination_IP , &newRequest.si_other.sin_addr) == 0)
            {
                newRequest.requestId=-1;
                if(std.DEBUG_STD_FATAL_ERRORS)printf("inet_aton() failed creating request with port id %d",newRequest.port);
                break;
                
            }else{
                newRequest.package = p;
                newRequest.requestId = p.localId;
                
                struct timeval tv;
//                tv.tv_sec       = ;
                tv.tv_usec      = 100;
                
                if (setsockopt(newRequest.s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
                    perror("send Message Socket Error");
                }
            }
       
        }
    }
    
    return newRequest;
}
int sendPackageWithRequest(uploadSocket sendRequest){
    
    int sendTries = SEND_TRIES;
    if (sendRequest.package.type==PACKAGE_TYPE_BROADCAST || sendRequest.package.type==PACKAGE_TYPE_BROADCAST_ERROR) {
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
            if(std.DEBUG_STD_NACK)printf("\Erro ao enviar mensagem para %d:%d",sendRequest.package.destinationId, sendRequest.port);
            return status;
        }else{
            status++;
            if(std.DEBUG_STD_SENDING)printf("\n-> %s",stringFromPackage(sendRequest.package));
        }
        //LIMPA O BUFFER DE ENVIO
        memset(sendRequest.buf,'\0', MAX_USER_MSG_SIZE);
        
        //AGUARDA RECEBIMENTO DO ACK
        if (recvfrom(sendRequest.s, sendRequest.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &sendRequest.si_other, &sendRequest.slen) == -1)
        {
            if(std.DEBUG_STD_NACK)printf("\n!-! FALHA na tentativa de receber ACK para o pacote %s",messageString);
            closeUp(sendRequest);

        }else{
            if(std.DEBUG_STD_ACK)printf("\n<=> Confirmação de recebimento: %s",   sendRequest.buf);
            
            Package p = packageFromString(sendRequest.buf);
            if (!(p.type==PACKAGE_TYPE_BROADCAST_ACK || p.type==PACKAGE_TYPE_BROADCAST_ERROR)) {
                receivingBuffer[receivingBufferIndex]=p;
                receivingBuffer[receivingBufferIndex].status=PACKAGE_STATUS_READY;
                receivingBufferIndex++;
            }
            closeUp(sendRequest);
            return ++status;
            break;
        }
    }
    return status;
}

void closeUp(uploadSocket up){
    close(up.s);
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
//    conn->downloadSocket.port = conn->selfRouter.port;
//    conn->downloadSocket.idNumber = conn->selfRouter.id; //teletar
//    conn->downloadSocket = initDownClient(conn->downloadSocket);
    pthread_t download_Singleton;
    
    pthread_create(&download_Singleton, NULL, startDownListen, NULL);
    return download_Singleton;
}

downloadSocket initDownClient(downloadSocket down){
    down = down;
    
    down.slen = sizeof(down.si_other);
    
    
    
    //CRIA O SOCKET
    if ((down.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        if(std.DEBUG_STD_FATAL_ERRORS)printf("\nAguardando socket livre para download..."); //teletar
        sleep(1);
        return initDownClient(down);
    }else{
        //ZERA A ESTRUTURA DO SOCKET
        memset((char *) &down.si_me, 0, sizeof(down.si_me));
        
        down.si_me.sin_family = AF_INET;
        down.si_me.sin_port = htons(down.port);
        down.si_me.sin_addr.s_addr = htonl(INADDR_ANY);
        
        //INICIA O SOCKET
        if( bind(down.s , (struct sockaddr*)&down.si_me, sizeof(down.si_me) ) == -1)
        {
            die("bind");
        }
    }
    
    
    return down;
}


void * startDownListen(void){
    
    while(1)
    {
        
        conn.downloadSocket.port = conn.selfRouter.port;
        conn.downloadSocket.idNumber = conn.selfRouter.id;
        conn.downloadSocket = initDownClient(conn.downloadSocket);

        fflush(stdout);
        //LIMPA O BUFFER DO SOCKET
        memset(conn.downloadSocket.buf,'\0', MAX_USER_MSG_SIZE);
        
        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        if (setsockopt(conn.downloadSocket.s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            perror("Error");
        }
        
        
        //CHAMADA BLOQUEANTE, RECEBE INFORMACOES
        if ((conn.downloadSocket.recv_len = recvfrom(conn.downloadSocket.s, conn.downloadSocket.buf, MAX_USER_MSG_SIZE, 0, (struct sockaddr *) &conn.downloadSocket.si_other, &conn.downloadSocket.slen)) == -1)
        {
            //printf("\nerro recvfrom() timeout\n");
        }else{
            //IMPRESSAO DO PACOTE RECEBIDO
            if(std.DEBUG_STD_RECEIVING)printf("\n<- %s" , conn.downloadSocket.buf);
            
            //ADICIONA NO BUFFER
            if (receivingBufferIndex==ROUTING_BUFFER_SIZE && receivingBuffer[1].status==PACKAGE_STATUS_DONE) {
                receivingBufferIndex=1;
            }else if(receivingBufferIndex==ROUTING_BUFFER_SIZE && receivingBuffer[1].status!=PACKAGE_STATUS_DONE){
                if(std.DEBUG_STD_FATAL_ERRORS)printf("!!!!!!BUFFER DE RECEBIMENTO CHEIO!!!!!");
                exit(0);
            }
            
            Package received = packageFromString(conn.downloadSocket.buf);
            received.status = PACKAGE_STATUS_READY;
            if (receivingBufferIndex>=ROUTING_BUFFER_SIZE) {
                receivingBufferIndex=1;
            }
            receivingBuffer[receivingBufferIndex]=received;
            receivingBufferIndex++;

            Package ackPkg = ackPackage(received);
            //RESPONDE O PACOTE COM O ACK
            if (sendto(conn.downloadSocket.s, stringFromPackage(ackPkg), conn.downloadSocket.recv_len, 0, (struct sockaddr*) &conn.downloadSocket.si_other, conn.downloadSocket.slen) == -1)
            {
                die("sendto()");
            }
            
        }
        
        closeDown(conn.downloadSocket);
    }
}


void closeDown(downloadSocket down){
    close(down.s);
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

void updateRoutingTableTimeStamp(){
    strcpy(conn.timestamp, time_stamp());
}

void updateRoutingTableWithPackage(Package p){
    char *token;
    char *rest = p.message;
    
    if (strcmp(p.timestamp, conn.timestamp)<=0 && p.type!=PACKAGE_TYPE_BROADCAST_ERROR) {
        if(std.DEBUG_STD_TIMESTAMP_ACK) printf("\n!!Pacote broadcast descartado");
    }else{
        if(std.DEBUG_STD_TIMESTAMP_ACK) printf("\nPacote broadcast aceito");
        
        while((token = strtok_r(rest, "|", &rest)))
        {
            char *packageToken;
            char *packagerest = token;
            
            char strLink[30];
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
                    case 3:
                        l.isDirectlyConnected=atoi(packageToken);
                        break;
                }
                i++;
            }
            l.remoteId=p.localId;
            int indxOfLink = indexOfLinkInConnections(l);
            if (p.type==PACKAGE_TYPE_BROADCAST_ERROR) {
                    removeAllId(l.from);
            }else{
                if (indxOfLink>=0) {
                    if (conn.linksList[indxOfLink].remoteId==p.localId||conn.linksList[indxOfLink].remoteId==99) {
                        if (((conn.linksList[indxOfLink].from==p.localId && conn.linksList[indxOfLink].to==conn.selfID)
                             ||(conn.linksList[indxOfLink].to==p.localId && conn.linksList[indxOfLink].to==conn.selfID))
                            
                            && conn.linksList[indxOfLink].isDirectlyConnected==0) {
                            conn.linksList[indxOfLink].isDirectlyConnected=1;
                        }
                        conn.linksList[indxOfLink].ttl=MAX_LINK_TTL;
                        updateRoutingTableTimeStamp();
                        
                    }
                }else{
                    if (isBetterLink(l, conn.routingTable[l.from][l.to])
                        &&isBetterLink(l, conn.routingTable[l.to][l.from])
                        &&isBetterLink(l, conn.routingTable[conn.selfID][l.to])
                        ) {
                        router r = routerOfIndex(l.to, conn.routerList); //l.from?!
                        if (r.id>=0 && (l.from==conn.selfID || l.to==conn.selfID) && r.port<0){
                            while (r.id>=0) {
                                removeRouter(&conn, r);
                                r = routerOfIndex(l.to, conn.routerList);
                            }
                        }
                        if (r.id<0 && (l.from==conn.selfID || l.to==conn.selfID)) {
                            r.port = p.senderPort;
                            strcpy(r.ip, p.senderIP);
                            r.id = p.localId;
                            addRouter(&conn, r);
                            l.isDirectlyConnected=1;
                        }else if(r.id<0 && !(l.from==conn.selfID || l.to==conn.selfID)){
                            router newr;
                            newr.id = l.to;
                            sprintf(newr.ip, "%d",l.from);
                            newr.port = -1;
                            l.isDirectlyConnected=0;
                            addRouter(&conn, newr);
                        }else{
                            l.isDirectlyConnected=0;
                        }
                        if (addLink(l)) {
                            if(std.DEBUG_STD_ROUTER_ADD_REMOVE) printf("\n!!Novo enlace adicionado!! %d - %d - %d\n", l.from, l.to, l.cost);
                            conn.routingTable[l.from][l.to]=l;
                            conn.routingTable[l.to][l.from]=l;
                            updateRoutingTableTimeStamp();
                            if(std.DEBUG_STD_ROUTING_TABLE) presentRoutingTable(conn.routingTable);
                        }
                        
                    }
                    
                }
            }
        }


    }
    
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
        if (pid>=receivingBufferIndex) {
            pid=0;
        }
        Package p = receivingBuffer[pid];
        if (p.status==PACKAGE_STATUS_READY||p.status==0) {
            p.status=PACKAGE_STATUS_PROCESSING;
            switch (p.type) {
                case PACKAGE_TYPE_BROADCAST:
                    updateRoutingTableWithPackage(p);
                    break;
                case PACKAGE_TYPE_BROADCAST_ERROR:
                    updateRoutingTableWithPackage(p);
                    break;

                case PACKAGE_TYPE_MESSAGE:
                    printMessagePackage(p);
                    break;
                case PACKAGE_TYPE_FORWARD:
                    printForwardPackage(p);
                    Package toRoute = routedPackage(packageFromString(p.message));
                    toRoute.packageId = getRequestIdForPackage();
                    addSendPackageToBuffer(toRoute);
                    break;
                case PACKAGE_TYPE_ACK:
                    setackPackage(p);
                break;

            }
            receivingBuffer[pid].status=PACKAGE_STATUS_DONE;

        }
        
    }
}

Package ackPackage(Package p){
    
    //inverte a direção do pacote
    p.destinationId += p.localId;
    p.localId = p.destinationId-p.localId;
    p.destinationId = p.destinationId - p.localId;
    strcpy(p.timestamp, time_stamp());
    router r = routerOfIndex(p.destinationId, conn.routerList);

    strcpy(p.destinationIP, r.ip);
    p.port = r.port;
    if (p.type==PACKAGE_TYPE_BROADCAST || p.type==PACKAGE_TYPE_BROADCAST_ERROR) {
        p.type=PACKAGE_TYPE_BROADCAST_ACK;
    }else{
        p.type = PACKAGE_TYPE_ACK;
    }
    strcpy(p.message, "");
    p.status = PACKAGE_STATUS_READY;
    
    return p;
}


Package packageFromString(char *s){

    char *token;
    char *rest = s;
    
    Package p;
    int i=0;
    while((token = strtok_r(rest, "@", &rest)))
    {
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
                strcpy(p.timestamp, token);
                break;
            case 4:
                strcpy(p.destinationIP, token);
                break;
            case 5:
                p.port = atoi(token);
                break;
            case 6:
                p.ttl = atoi(token);
                break;
            case 7:
                p.type = atoi(token);
                break;
            case 8:
                p.senderPort = atoi(token);
                break;
            case 9:
                strcpy(p.senderIP, token);
                if (p.type==PACKAGE_TYPE_FORWARD) {
                    strcpy(p.message, rest);
                }
                break;
            case 10:
                strcpy(p.message, token);
                break;
        }
        if (i==9 && p.type==PACKAGE_TYPE_FORWARD) break;
        i++;
        
    }
    p.status = PACKAGE_STATUS_READY;

    
    return p;
}


char * stringFromPackage(Package p){
    char *str;
    char separator = '@';
    asprintf(&str, "%d%c%d%c%d%c%s%c%s%c%d%c%d%c%d%c%d%c%s%c%s",
             p.localId,separator,
             p.destinationId,separator,
             p.packageId,separator,
             p.timestamp,separator,
             p.destinationIP,separator,
             p.port,separator,
             p.ttl,separator,
             p.type,separator,
             p.senderPort,separator,
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
        if (/*l.to!=conn.linksList[y].from && */l.to!=conn.linksList[y].to && l.to!=conn.linksList[y].remoteId) {
            asprintf(&str, "%s%d-%d-%d|",str,conn.linksList[y].from,conn.linksList[y].to,conn.linksList[y].cost+l.cost);
        }
        y--;
    }

    return str;
}


char * getLinkStringToDelete(connections conn, int l){
    char *str = "";
    asprintf(&str, "%s%d-%d-%d-%d|",str,l,l,l, BROADCAST_CODE_LINK_ERROR);
    return str;
}


void cancelAllBroadcastForDelete(int l){
    for (int index = 0; index<sendingBufferIndex; index++) {
        if (sendingBuffer[index].status==PACKAGE_STATUS_READY && sendingBuffer[index].type == PACKAGE_STATUS_READY) {
            sendingBuffer[index].status=PACKAGE_STATUS_DEAD;
        }
    }
    for (int x = 0; x<conn.linksCount; x++) {
        
    if (conn.linksCount>0 && conn.linksList[x].isDirectlyConnected==1) {
            Package p;
            p.localId = conn.selfID;
            p.destinationId = conn.linksList[x].to;
            p.packageId = getRequestIdForPackage();
            strcpy(p.destinationIP, routerOfIndex(conn.linksList[x].to, conn.routerList).ip);
            p.ttl=MAX_TTL_BROADCAST;
            p.type=PACKAGE_TYPE_BROADCAST_ERROR;
            p.senderPort=conn.selfRouter.port;
            strcpy(p.senderIP, conn.selfRouter.ip);
            strcpy(p.timestamp, time_stamp());
            strcpy(p.message, getLinkStringToDelete(conn, l));
            p.status=PACKAGE_STATUS_READY;
            p.port = routerOfIndex(conn.linksList[x].to, conn.routerList).port;
            addSendPackageToBuffer(p);
        }
    }

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
            p.senderPort=conn.selfRouter.port;
            strcpy(p.senderIP, conn.selfRouter.ip);
            strcpy(p.timestamp, time_stamp());
            strcpy(p.message, getLinkStringToBroadCast(conn,conn.linksList[x]));
            p.status=PACKAGE_STATUS_READY;
            p.port = routerOfIndex(conn.linksList[x].to, conn.routerList).port;
            addSendPackageToBuffer(p);
        }else{
            conn.linksList[x].ttl--;
            if (conn.linksList[x].ttl<0 && conn.linksList[x].isDirectlyConnected==0) {
                removeAllId(conn.linksList[x].to);
                removeAllId(conn.linksList[x].from);
            }
        }
        if (x>=conn.linksCount-1) {
            sleep(3);
            if (conn.linksCount<=1) {
                sleep(5);
                if (conn.linksCount<=1) {
                    prepareRoutingTable(&conn);
                    //CONFIG FILE READING
                    conn = readLinks(PATH_LINKS_FILE, conn);
                    conn = readRouters(PATH_ROUTER_FILE, conn);
                    addRouter(&conn, conn.selfRouter);
                }

            }
            x=0;
        }else{
            x++;
        }


    }
    return NULL;
}

void initRouting(){
    printf("Iniciando envio de pacotes com informações de enlace em 5 segundos.");
    sleep(5);
    pthread_t sendLinksBroadcastSingleton;
    pthread_create(&sendLinksBroadcastSingleton, NULL, sendLinksBroadcast, NULL);
}


linkr connectedLinkToDestinationId(int from, int to, int deep){
    linkr shortestNext;
    shortestNext.cost=999;
    for (int r=0; r<conn.linksCount; r++) {
        if ((conn.linksList[r].to==to||conn.linksList[r].from==to)&&conn.linksList[r].cost<shortestNext.cost) {
            shortestNext=conn.linksList[r];
        }
    }
    if (deep>0 && from==conn.selfID) {
        removeAllId(from);
        linkr nil;
        nil.cost=999;
        nil.from=nil.to=nil.ttl=nil.remoteId=nil.to=0;
        return nil;
//        removeAllId(to); //teletar
    }
    if (shortestNext.isDirectlyConnected) {
        return shortestNext;
    }else{
        return connectedLinkToDestinationId(from, shortestNext.from, deep++);
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
    if (p.type==PACKAGE_TYPE_BROADCAST || p.type==PACKAGE_TYPE_BROADCAST_ERROR) {
        return p;
    }
    linkr l= connectedLinkToDestinationId(p.localId, p.destinationId,0);
    
    if ((l.isDirectlyConnected==1&&(p.destinationId==l.to||p.destinationId==l.from))) {
        router r = routerOfIndex(l.to, conn.routerList);
        p.port = r.port;
        strcpy(p.destinationIP, r.ip);
        return p;
    }else{
        router r = routerOfIndex(indexForBidirectionalLink(l), conn.routerList);
        Package routedPackage;
        
        if (r.id<0 && strlen(r.ip)==0) {
            routedPackage.status=PACKAGE_STATUS_DEAD;
            return routedPackage;
        }else if(p.type==PACKAGE_TYPE_MESSAGE){
            routedPackage.status=PACKAGE_STATUS_READY;
            routedPackage.localId = conn.selfID ;
            routedPackage.destinationId = l.to;
            routedPackage.packageId = getRequestIdForPackage();
            strcpy(routedPackage.destinationIP, r.ip);
            routedPackage.ttl=p.ttl--;
            strcpy(routedPackage.timestamp, time_stamp());
            routedPackage.type=PACKAGE_TYPE_FORWARD;
            routedPackage.senderPort=conn.selfRouter.port;
            strcpy(routedPackage.senderIP, conn.selfRouter.ip);
            strcpy(routedPackage.message, stringFromPackage(p));
            routedPackage.port = r.port;
        
        }
        
        return routedPackage;
    }
}

void * flushSendBuffer(){
    while (1) {
        for (int index = 0; sendingBufferIndex; index++) {
            if (index>sendingBufferIndex) {
                index=0;
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
                        
                        if(std.DEBUG_STD_NACK)printf("\n!! ERRO AO ENVIAR PACOTE %s",stringFromPackage(sendRequest.package));
                        sendingBuffer[index].ttl--;
                        
                        //   exit(0);
                        break;
                    case REQUEST_STATUS_SEND_NO_ANSWER:
                        sendingBuffer[index].ttl--;
                        if (sendingBuffer[index].ttl<=0) {
                            sendingBuffer[index].status= PACKAGE_STATUS_DEAD;
                            if(std.DEBUG_STD_NACK) printf("\n!!-!! NAO É POSSIVEL ENVIAR PARA ESTE DESTINO %s",stringFromPackage(sendingBuffer[index]));
                            removeAllId(sendingBuffer[index].destinationId);
                        }else if (sendingBuffer[index].status!=PACKAGE_STATUS_DEAD) {
                                sendingBuffer[index].status=PACKAGE_STATUS_READY;
                                sendingBuffer[index].ttl--;
                        }
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
    printf("| ID: %d | Porta: %d | DESTINO: %s\n",r.id,r.port,r.ip);
}

void printRouters(connections conn){
    for (int x=0; x<conn.routerCount; x++) {
        printRouter(conn.routerList[x]);
    }
}

void printlink(linkr l){
    if (l.isDirectlyConnected==1) {
        printf("||FROM: %d | TO: %d | COST: %d | TTL: %d\n",l.from,l.to,l.cost, l.ttl);
    }else{
        printf("| FROM: %d | TO: %d | COST: %d | TTL: %d\n",l.from,l.to,l.cost, l.ttl);
    }
}
void printLinks(connections conn){
    for (int x=0; x<conn.linksCount; x++) {
        printlink(conn.linksList[x]);
    }
}

router chooseDestination(){
    router r;
    r.id=-1;
    while (r.id<0) {
        printf("\n----------------------\n");
        for (int x=0; x<conn.routerCount; x++) {
            printf(" %d ",conn.routerList[x].id);
        }

        char option[10] = "";
        
        printf("\nEscolha o id do destinatário: ");
        scanf("%s",option);
        int indx = atoi(option);
        if (indx>0) {
            r=routerOfIndex(indx, conn.routerList);
        }else{
            break;
        }
    }
    return r;
}

void chat(struct router destinationRouter){

    char message[MAX_USER_MSG_SIZE];
    
    sleep(1);
//    while (1) {
        int msgid=getRequestIdForPackage();
        
        printf("\n( :menu to exit) Insira o conteúdo da mensagem id %d para roteador id %d: ", msgid, destinationRouter.id);
        scanf("%s",message);
        if (strcmp(":menu", message)==0) {
//            break;
        }
        
        Package p;
        p.localId = conn.selfID;
        p.destinationId = destinationRouter.id;
        p.packageId =msgid;
        strcpy(p.destinationIP, destinationRouter.ip);
        p.ttl=MAX_TTL;
        p.type=PACKAGE_TYPE_MESSAGE;
        strcpy(p.timestamp, time_stamp());
        strcpy(p.senderIP, conn.selfRouter.ip);
        p.senderPort  = conn.selfRouter.port;
        p.status=PACKAGE_STATUS_READY;
        strcpy(p.message, message);
        p.port = destinationRouter.port;
        addSendPackageToBuffer(p);

        
        
//    }
}

void removeAllId(int idx){
    for (int x=0; x<conn.routerCount; x++) {
        char *ipid;
        asprintf(&ipid, "%d",idx);

        if (conn.routerList[x].id==idx /*|| strcmp(conn.routerList[x].ip,ipid)==0*/) {
            removeRouter(&conn, conn.routerList[x]);
            if(std.DEBUG_STD_LINK_ADD_REMOVE)printf("\n!!No %d removido!!", idx);
        }
    }
    for (int x=0; x<conn.linksCount; x++) {
        if (conn.linksList[x].to==idx || conn.linksList[x].from==idx) {
            linkr newLink;
            newLink.from=newLink.to=newLink.ttl=0;
            newLink.cost=99;
            conn.routingTable[conn.linksList[x].to][conn.linksList[x].from]=newLink;
            conn.routingTable[conn.linksList[x].from][conn.linksList[x].to]=newLink;
            if (x==conn.linksCount-1) {
                conn.linksList[x]=newLink;
                conn.linksCount--;
            }else{
                conn.linksList[x]=conn.linksList[conn.linksCount-1];
                conn.linksList[conn.linksCount-1]=newLink;
                conn.linksCount--;
            }
            cancelAllBroadcastForDelete(idx);

            if(std.DEBUG_STD_LINK_ADD_REMOVE) printf("\n!!Enlace para %d removido!!", idx);
            presentRoutingTable(conn.routingTable);
        }
    }
    updateRoutingTableTimeStamp();

}

void presentRoutingTable(struct linkr routingTable[MAX_LINKS][MAX_LINKS]){
   
    printf("\n\n=====[TABELA DE ROTEAMENTO]===== Timestamp: %s\n\n   ",conn.timestamp);
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
    printf("\n\n");
        
}

void interface(){
    char option[10] = ":menu";
    while (1) {

            if (strcmp(option, "1")==0) {
                router r;
                r = chooseDestination();
                if (r.id>0) {
                    chat(r);
                }
                
            }
            if (strcmp(option, "2")==0) {
                println(5);
                printf("\n\n=====[Tabela de roteamento]=====\n");
                printLinks(conn);
                printf("=====[Detalhamento dos nos encontrados]=====\n");
                printRouters(conn);
            }
            if (strcmp(option, "3")==0) {
                presentRoutingTable(conn.routingTable);
            }
            if (strcmp(option, "4")==0) {
                println(5);
                printf("\n\n=====[Tabela de roteamento]=====\n");
                printLinks(conn);
                printf("=====[Detalhamento dos nos encontrados]=====\n");
                printRouters(conn);
                
                printf("\n [ 1 ] Adicionar Link");
                printf("\n [ 2 ] Remover Link");
                printf("\n [ 3 ] Editar Link");

                scanf("%s",option);
                if (strcmp(option, "1")==0) {
                    linkr l;
                    l.from=l.to=-1;
                    addLink(l);
                }
                if (strcmp(option, "2")==0) {
                    editLink(&conn, "");
                }
                if (strcmp(option, "3")==0) {
                    removeLink(&conn, "");
                }
                
            }
            if (strcmp(option, "5")==0) {
                printf("\n\n=====[Debug]=====\n");
                printf("\n 1  DEBUG_STD_SENDING %d ",std.DEBUG_STD_SENDING);
                printf("\n 2  DEBUG_STD_RECEIVING %d ",std.DEBUG_STD_RECEIVING);
                printf("\n 3 DEBUG_STD_ACK %d ",std.DEBUG_STD_ACK);
                printf("\n 4 DEBUG_STD_TIMESTAMP_ACK %d ",std.DEBUG_STD_TIMESTAMP_ACK);
                printf("\n 5 DEBUG_STD_ROUTER_ADD_REMOVE %d ",std.DEBUG_STD_ROUTER_ADD_REMOVE);
                printf("\n 6 DEBUG_STD_LINK_ADD_REMOVE %d ",std.DEBUG_STD_LINK_ADD_REMOVE);
                printf("\n 7 DEBUG_STD_ROUTING_TABLE %d ",std.DEBUG_STD_ROUTING_TABLE);
                printf("\n 8 DEBUG_STD_NACK %d ",std.DEBUG_STD_NACK);
                printf("\n 9 DEBUG_STD_FATAL_ERRORS %d \n\nInforme o debug que deseja alterar: ",std.DEBUG_STD_FATAL_ERRORS);
                int op=0;
                scanf("%d",&op);
                switch (op) {
                    case 1:
                        std.DEBUG_STD_SENDING=!std.DEBUG_STD_SENDING;
                        break;
                    case 2:
                        std.DEBUG_STD_RECEIVING=!std.DEBUG_STD_RECEIVING;
                        break;

                    case 3:
                        std.DEBUG_STD_ACK=!std.DEBUG_STD_ACK;
                        break;

                    case 4:
                        std.DEBUG_STD_TIMESTAMP_ACK=!std.DEBUG_STD_TIMESTAMP_ACK;
                        break;

                    case 5:
                        std.DEBUG_STD_ROUTER_ADD_REMOVE=!std.DEBUG_STD_ROUTER_ADD_REMOVE;
                        break;

                    case 6:
                        std.DEBUG_STD_LINK_ADD_REMOVE=!std.DEBUG_STD_LINK_ADD_REMOVE;
                        break;

                    case 7:
                        std.DEBUG_STD_ROUTING_TABLE=!std.DEBUG_STD_ROUTING_TABLE;
                        break;

                    case 8:
                        std.DEBUG_STD_NACK=!std.DEBUG_STD_NACK;
                        break;

                    case 9:
                        std.DEBUG_STD_FATAL_ERRORS=!std.DEBUG_STD_FATAL_ERRORS;
                        break;

                }
            }
        

        
            println(5);
            printf("\n\n==================[ROTEADOR SOCKET UPD]==================\n");
            printf("[ 1 ] Enviar mensagem \n[ 2 ] Enlaces e nos conectados \n[ 3 ] Tabela de roteamento \n");
            printf("[ 4 ] Alterar enlaces \n[ 5 ] Alterar stdout de debug \nSelecione um item:");
        
        scanf("%9s",option);
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

        std.DEBUG_STD_ACK=std.DEBUG_STD_FATAL_ERRORS=std.DEBUG_STD_LINK_ADD_REMOVE=std.DEBUG_STD_NACK=std.DEBUG_STD_RECEIVING=std.DEBUG_STD_ROUTER_ADD_REMOVE=std.DEBUG_STD_ROUTING_TABLE=std.DEBUG_STD_SENDING=std.DEBUG_STD_TIMESTAMP_ACK = 1;
        
        conn.selfID = atoi(argv[1]);
        
        autoIncrementalLocalRequestId=0;
        sendingBufferIndex=receivingBufferIndex=0;
        
        prepareRoutingTable(&conn);
        
        //CONFIG FILE READING
        conn = readLinks(PATH_LINKS_FILE, conn);
        conn = readRouters(PATH_ROUTER_FILE, conn);
        addRouter(&conn, conn.selfRouter);

        //pthread_t downloadSocketSingleton = //teletar
        initDownloadSocketThread(&conn);

        
        pthread_t routingSingleton;
        pthread_create(&routingSingleton, NULL, routing, NULL);

        pthread_t flushSendBufferSingleton;
        pthread_create(&flushSendBufferSingleton, NULL, flushSendBuffer, NULL);
      
        initRouting();
        
        sleep(1); // tempo para o socket de download ser inicializado
        interface();
    }
    return 0;
}
