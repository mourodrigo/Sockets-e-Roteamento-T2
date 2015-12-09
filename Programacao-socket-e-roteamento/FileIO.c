//
//  FileIO.c
//  
//
//  Created by Rodrigo Bueno Tomiosso on 03/10/15.
//
//

#include "FileIO.h"

char *time_stamp(){
    
    char *timestamp = (char *)malloc(sizeof(char) * 16);
    time_t ltime;
    ltime=time(NULL);
    struct tm *tm;
    tm=localtime(&ltime);
    
    sprintf(timestamp,"%04d%02d%02d%02d%02d%02d", tm->tm_year+1900, tm->tm_mon,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return timestamp;
}


char * replace(
               char const * const original,
               char const * const pattern,
               char const * const replacement
               ) {
    size_t const replen = strlen(replacement);
    size_t const patlen = strlen(pattern);
    size_t const orilen = strlen(original);
    
    size_t patcnt = 0;
    const char * oriptr;
    const char * patloc;
    
    // find how many times the pattern occurs in the original string
    for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen)
    {
        patcnt++;
    }
    
    {
        // allocate memory for the new string
        size_t const retlen = orilen + patcnt * (replen - patlen);
        char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );
        
        if (returned != NULL)
        {
            // copy the original string,
            // replacing all the instances of the pattern
            char * retptr = returned;
            for (oriptr = original; (patloc = strstr(oriptr, pattern)); oriptr = patloc + patlen)
            {
                size_t const skplen = patloc - oriptr;
                // copy the section until the occurence of the pattern
                strncpy(retptr, oriptr, skplen);
                retptr += skplen;
                // copy the replacement
                strncpy(retptr, replacement, replen);
                retptr += replen;
            }
            // copy the rest of the string.
            strcpy(retptr, oriptr);
        }
        return returned;
    }
}


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

connections readRouters(char filepath[], connections conn){

    char line[80];
    FILE *fr;

    printf ("\nReading file path %s \n", filepath);
    
    fr = fopen (filepath, "r");
    
    if (!fr) {
        printf("\n\nARQUIVO DE CONFIGURAÇÃO DE ROTEADORES NÃO ENCONTRADO. DEFINA O CAMINHO EM : Default.h \n\n");
        printf("\n\nInicializando sem arquivo de configuração, novos roteadores serao adicionados caso receba comunicacao dos enlaces\n\n");
        return conn;
    }

    
    int indx = 0;
    while(fgets(line, 240, fr) != NULL)
    {
#ifdef DEBUG_LEVEL_3
//        printf("Read Router link %s",line);
#endif
        strtok(line, "\n");
        char** tokens;
        router r;
        tokens = str_split(line, ' ');
        r.id = atoi(tokens[0]);
        r.port = atoi(tokens[1]);
        strcpy(r.ip, tokens[2]);
        
        int aux=conn.linksCount-1;
        while (aux>=0) {
            if (r.id==conn.linksList[aux].to) {
                conn.routerList[indx]=r;
                indx++;
            }
            if (r.id==conn.selfID) {
                conn.selfRouter=r;
            }
            aux--;
        }
    }
    fclose(fr);
    conn.routerCount = indx;
    return conn;
}



linkr linkFromChar(char *text, char *separator){
    
    char *packageToken;
    char *packagerest = text;
    
    linkr l;
    l.from=l.cost=l.to=l.isDirectlyConnected=0;
    int i=0;
    while((packageToken = strtok_r(packagerest, separator, &packagerest))){
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
    
    return l;

}

connections readLinks(char filepath[],connections conn){

    char line[80];
    FILE *fr;
    
#ifdef DEBUG_LEVEL_2
    printf ("\nReading file path %s \n", filepath);
#endif
    
    fr = fopen (filepath, "r");
    
    if (!fr) {
        printf("\n\nARQUIVO DE CONFIGURAÇÃO DE ENLACES NÃO ENCONTRADO. DEFINIR O CAMINHO EM : Default.h \n\n");
        printf("\n\nInicializando sem arquivo de configuração, novos enlaces serao adicionados caso receba comunicacao dos enlaces\n\n");
        return conn;
    }
    
    int indx = 0;
    while(fgets(line, 240, fr) != NULL)
    {
#ifdef DEBUG_LEVEL_3
//        printf("Read link %s",line);
#endif
        linkr l = linkFromChar(line, " ");
        l.isDirectlyConnected=1;
        l.remoteId=99;
        if (l.to==conn.selfID) {
            l.to=l.from;
            l.from=conn.selfID;
        }
        if (l.from==conn.selfID) {
            conn.linksList[indx]=l;
            indx++;
            conn.routingTable[l.from][l.to]=l;
            conn.routingTable[l.to][l.from]=l;
        }
        
    }
    
    fclose(fr);
    conn.linksCount = indx;
    return conn;
}

