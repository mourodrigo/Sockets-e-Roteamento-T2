//
//  FileIO.c
//  
//
//  Created by Rodrigo Bueno Tomiosso on 03/10/15.
//
//

#include "FileIO.h"

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    
    /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
    count++;
    
    result = malloc(sizeof(char*) * count);
    
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    
    return result;
}

int readRouters(char filepath[],struct router routers[MAX_ROUTERS]){

    char line[80];
    FILE *fr;

#ifdef DEBUG_LEVEL_2
    printf ("Reading file path %s \n", filepath);
#endif
    
    fr = fopen (filepath, "r");
    
    int indx = 0;
    while(fgets(line, 240, fr) != NULL)
    {
#ifdef DEBUG_LEVEL_3
        printf("Read Router link %s",line);
#endif
        char** tokens;
        router r;
        tokens = str_split(line, ' ');
        r.id = (int)tokens[0];
        r.port = (int)tokens[1];
        strcpy(r.ip, tokens[2]);
        
        routers[indx]=r;
        
        indx++;
    }
    fclose(fr);
    return indx;
}

int readLinks(char filepath[],struct linkr links[MAX_LINKS]){

    char line[80];
    FILE *fr;
    
#ifdef DEBUG_LEVEL_2
    printf ("Reading file path %s \n", filepath);
#endif
    
    fr = fopen (filepath, "r");
    
    int indx = 0;
    while(fgets(line, 240, fr) != NULL)
    {
#ifdef DEBUG_LEVEL_3
        printf("Read link %s",line);
#endif
        char** tokens;
        linkr l;
        tokens = str_split(line, ' ');

        l.from = atoi(tokens[0]);
        l.to = atoi(tokens[1]);
        l.cost = atoi(tokens[2]);
        
        links[indx]=l;
        
        indx++;
    }
    fclose(fr);
    return indx;
}

