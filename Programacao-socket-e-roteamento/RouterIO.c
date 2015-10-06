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
    
        printf("Enter message : ");
        //gets(message);
        scanf("%s",up.message);
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
