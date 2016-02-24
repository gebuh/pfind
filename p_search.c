/* Project: pfind
 * Author:  Beth Boose
 * File:    search.c
 * Purpose: functions to open/close, traverse and find files
 */

#include "p_search.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include<string.h>
#include <unistd.h>
#include <errno.h>

/* open a directory, iterate thru it, close it
 * 
 *  
 */
void search_dir ( char * dirname ) {
    DIR *dir_ptr;             /* an actual directory */
    
    /* open the directory with name dirname, if not report and return 
     * not necessarily a failure
     */
    dir_ptr = opendir(dirname);
    if(dir_ptr == NULL){
        report_err ("search_dir", "opendir");
        return;
    }
    
    for (;;) {
       struct dirent entry;
       struct dirent *curr_dir;
       char *name;
       char sub_dir[256];
       
       int err = readdir_r(dir_ptr, &entry, &curr_dir);
       if ( err != EXIT_SUCCESS ) {
           report_err ("search_dir", "read dir"); //FIXME - KEEP GOING?
           return;
       }
       
       if (curr_dir == NULL ) {
           printf( "reached the end of this path\n ");
           break;
       }
       
       name = curr_dir->d_name;
       if ((strcmp(name, ".") == 0 ) || (strcmp (name, "..") == 0)) {
           continue; /* skip self and parents */
       }
       
       if (curr_dir->d_type == DT_DIR ) { /* only recurse thru directories */
           sprintf ( sub_dir, "%s%s", dirname, name ); /* RECURSION!!! */
       }
    }
    
    closedir(dir_ptr); //FIXME - ERROR HANDLING
}



/* error handling */
void report_err (char *func_name, char *func_param ) {
    char curr_dir[PATH_MAX + 1];
    
    fprintf ( stderr, "pfind in %s/ %s(%s) failed: %s\n",
            getcwd ( curr_dir, sizeof(curr_dir)),
            func_name,
            func_param,
            strerror (errno)
    );
}