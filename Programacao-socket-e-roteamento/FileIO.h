//
//  FileIO.h
//  
//
//  Created by Rodrigo Bueno Tomiosso on 03/10/15.
//
//

#ifndef ____FileIO__
#define ____FileIO__

#include <stdio.h>   /* required for file operations */
#include <string.h>
#include <assert.h>
#include "Defaults.h"
int readRouters(char filepath[],struct router routers[MAX_ROUTERS]);
int readLinks(char filepath[],struct linkr links[MAX_LINKS]);
char** str_split(char* a_str, const char a_delim);
char * replace(
               char const * const original,
               char const * const pattern,
               char const * const replacement
               );
#endif /* defined(____FileIO__) */
