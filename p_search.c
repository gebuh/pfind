/* Project: pfind
 * Author:  Beth Boose
 * File:    search.c
 * Purpose: functions to open/close, traverse and find files
 */

#include "p_search.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <glob.h>



/* open a directory, iterate thru it, close it
 * 
 *  
 */
void search_dir( char *parentdir, char * dirname, struct opt_info * opts, int depth ) {
    DIR *dir_ptr; /* an actual directory */
    char currpath[PATH_MAX +1]; 
    int num_glob_call = 0; /* ugly way to keep glob from running too many times */
    depth+= 1;
    if (parentdir != NULL && parentdir[0] != '\0') {
        sprintf ( currpath, "%s/%s", parentdir, dirname); // keep current full path
    } else {
        strcpy ( currpath, dirname );
    }
    
    /* open the directory with name dirname, if not report and return 
     * not necessarily a failure
     */
    dir_ptr = opendir(dirname);
    
    if (!dir_ptr) {
        report_err("opendir", dirname);
        return;
    }

    int cherr = chdir(dirname);
    if (cherr != 0) {
        report_err("chdir", dirname); //FIXME - KEEP GOING?
        closedir(dir_ptr); // don't care about errors
        return;
    }

    for (;;) {
        struct dirent entry;
        struct dirent *next_dir;
        char *name;

        int err = readdir_r(dir_ptr, &entry, &next_dir);
        if (err != 0) {
            report_err("readdir", dirname); //FIXME - KEEP GOING?
            continue;
        }

        if (next_dir == NULL) {
            break;
        }
        

        name = next_dir->d_name;
        if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) {
            continue; /* skip self and parents */
        } else {
            glob_t gbuf;
            
            memset ( &gbuf, 0, sizeof ( gbuf ));
            if ( (num_glob_call == 0) && glob( opts->name_arg, 0, NULL,&gbuf ) == 0 ){
                for ( size_t i=0; i<gbuf.gl_pathc; i++){
                   printf("%s/%s\n", currpath, gbuf.gl_pathv[i]); 
                }
            }
            num_glob_call++;
        }

        if (next_dir->d_type == DT_DIR) { /* only recurse thru directories */
            search_dir(currpath, name, opts, depth); /* RECURSION!!! */
        }
    }

    closedir(dir_ptr); //FIXME - ERROR HANDLING
    
    cherr = chdir("..");
    if (cherr != 0) {
        report_err("chdir", ".."); //FIXME - KEEP GOING?
        return;
    }
    
}

/* error handling */
void report_err(char *func_name, char *func_param) {
    char curr_dir[PATH_MAX + 1];

    fprintf(stderr, "pfind in %s/ %s(%s) failed: %s\n",
            getcwd(curr_dir, sizeof (curr_dir)),
            func_name,
            func_param,
            strerror(errno)
            );
}