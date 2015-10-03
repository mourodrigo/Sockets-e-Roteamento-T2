//
//  main.c
//  Programacao-socket-e-roteamento
//
//  Created by Rodrigo Bueno Tomiosso on 01/10/15.
//  Copyright (c) 2015 mourodrigo. All rights reserved.
//

#include "Defaults.h"

typedef struct router{
    int id;
    int port;
    char ip[15];
}router;

int main(int argc, const char * argv[]) {
    // insert code here...
    
    char *new_str;
    asprintf(&new_str,"%s%s",argv[0],PATH_ROUTER_FILE);
    
#if defined(_AIX)
    printf("UNIX");
#endif
    
    printf("Hello there! Executing from: \n %s \n",new_str);
    
    
    
    return 0;
}
