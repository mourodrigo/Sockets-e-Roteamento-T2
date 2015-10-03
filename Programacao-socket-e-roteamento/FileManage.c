//
//  FileManage.c
//  
//
//  Created by Rodrigo Bueno Tomiosso on 03/10/15.
//
//

#include "FileManage.h"

void readFile(char filepath[]){
    int n;
    long elapsed_seconds;
    char line[80];
    FILE *fr;

    printf ("Reading file path %s \n", filepath);
    
    fr = fopen (filepath, "rt");  /* open the file for reading */
    /* elapsed.dta is the name of the file */
    /* "rt" means open the file for reading text */
    
    while(fgets(line, 80, fr) != NULL)
    {
        /* get a line, up to 80 chars from fr.  done if NULL */
        sscanf (line, "%ld", &elapsed_seconds);
        /* convert the string to a long int */
        printf ("%ld\n", elapsed_seconds);
    }
    fclose(fr);
}