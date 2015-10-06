//
//  RouterIO.c
//  
//
//  Created by Rodrigo Bueno Tomiosso on 06/10/15.
//
//

#include "RouterIO.h"

void muerte(char *s)
{
    perror(s);
    exit(1);
}

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
        up.si_other.sin_port = htons(PORT);
        
        if (inet_aton(SERVER , &up.si_other.sin_addr) == 0)
        {
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }
        
//    }
    return up;
}
void sendMessage(RouterUp up){
    
//        printf("Enter message : ");
        //gets(message);
//        scanf("%s",up.message);
        //send the message
        if (sendto(up.s, up.message, strlen(up.message) , 0 , (struct sockaddr *) &up.si_other, up.slen)==-1)
        {
            muerte("sendto()");
        }
        
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(up.buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(up.s, up.buf, BUFLEN, 0, (struct sockaddr *) &up.si_other, &up.slen) == -1)
        {
            muerte("recvfrom()");
        }
        
        puts(up.buf);
    
}

void closeUp(RouterUp up){
    close(up.s);
#ifdef DEBUG_LEVEL_3
    printf("Up Client Closed\n");
#endif
}



RouterDown initDownClient(RouterDown down){
    
    //create a UDP socket
    if ((down.s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        muerte("socket");
    }
    
    // zero out the structure
    memset((char *) &down.si_me, 0, sizeof(down.si_me));
    
    down.si_me.sin_family = AF_INET;
    down.si_me.sin_port = htons(PORT_RECEIVE);
    down.si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if( bind(down.s , (struct sockaddr*)&down.si_me, sizeof(down.si_me) ) == -1)
    {
        muerte("bind");
    }
    
 
    
    
    return down;
}

void routerDidReceiveMessage(char message[1025]){
    printf("\n (Server side) sending %s \n", message);
    //    write(down.connfd, down.sendBuff, strlen(down.sendBuff));
    
}

void startDownListen(RouterDown down){
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(down.buf,'\0', BUFLEN);
        
        //try to receive some data, this is a blocking call
        if ((down.recv_len = recvfrom(down.s, down.buf, BUFLEN, 0, (struct sockaddr *) &down.si_other, &down.slen)) == -1)
        {
            muerte("recvfrom()");
        }
        
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(down.si_other.sin_addr), ntohs(down.si_other.sin_port));
        printf("Data: %s\n" , down.buf);
        
        //now reply the client with the same data
        if (sendto(down.s, down.buf, down.recv_len, 0, (struct sockaddr*) &down.si_other, down.slen) == -1)
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

