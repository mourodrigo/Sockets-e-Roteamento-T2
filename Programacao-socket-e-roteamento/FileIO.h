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
#include "RouterIO.h"

connections readRouters(char filepath[], connections conn);
connections readLinks(char filepath[],connections conn);
char** str_split(char* a_str, const char a_delim);
char * replace(
               char const * const original,
               char const * const pattern,
               char const * const replacement
               );
linkr linkFromChar(char *text, char *separator);
#endif /* defined(____FileIO__) */
