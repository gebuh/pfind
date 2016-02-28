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
#include <sys/stat.h>

/* recursively iterate through starting directory 
 *  print out matching resources/types
 */
void search_dir(char *parentdir, char * dirname, struct opt_info * opts) {
    DIR *dir_ptr; /* an actual directory */
    char currpath[PATH_MAX + 1];
    int num_glob_call = 0; /* keep glob from running too many times */

    if (parentdir != NULL && parentdir[0] != '\0') {
        /* keep current full path */
        sprintf(currpath, "%s/%s", parentdir, dirname);
    } else {
        strcpy(currpath, dirname);
    }

    /* open the directory with name dirname, if not report and return 
     * not necessarily a failure
     */
    dir_ptr = opendir(dirname);

    if (!dir_ptr) {
        report_func_err(dirname);
        return;
    }

    int cherr = chdir(dirname);
    if (cherr != 0) {
        report_err("chdir", dirname);
        closedir(dir_ptr); // don't care about errors
        return;
    }

    for (;;) {
        struct dirent entry;
        struct dirent *next_dir;
        char *name;

        int err = readdir_r(dir_ptr, &entry, &next_dir);
        if (err != 0) {
            /* readdir fails silently */
            /* report_err("readdir", dirname); */
            continue;
        }

        if (next_dir == NULL) {
            break;
        }


        name = next_dir->d_name;
        if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) {
            continue; /* skip self and parents */
        } else {
            filter_results(opts, currpath, &num_glob_call);
        }

        if (next_dir->d_type == DT_DIR) { /* only recurse thru directories */
            search_dir(currpath, name, opts); /* RECURSION!!! */
        }
    }

    closedir(dir_ptr);

    cherr = chdir("..");
    if (cherr != 0) {
        report_err("chdir", "..");
        return;
    }

}

/* error handling */
void report_err(char *func_name, char *func_param) {
    char curr_dir[PATH_MAX + 1];

    fprintf(stderr, "pfind: `%s/%s(%s)': %s\n",
            getcwd(curr_dir, sizeof (curr_dir)),
            func_name,
            func_param,
            strerror(errno)
            );
}

/* error handling */
void report_func_err(char *func_param) {
    char curr_dir[PATH_MAX + 1];

    fprintf(stderr, "pfind: `%s/%s': %s\n",
            getcwd(curr_dir, sizeof (curr_dir)),
            //func_name,
            func_param,
            strerror(errno)
            );
}

/**
 * filter on name and type
 * @param opts
 * @param currpath
 * @param num_glob_call
 */
void filter_results(struct opt_info * opts, char *currpath,
        int *num_glob_call) {
    glob_t gbuf;
    int typematches = 1;
    memset(&gbuf, 0, sizeof ( gbuf));
    if (((*num_glob_call) == 0) && glob(opts->name_arg, 0, NULL, &gbuf) == 0) {
        for (size_t i = 0; i < gbuf.gl_pathc; i++) {
            if (opts->type_flag) { /* looking for type? */
                struct stat pstat;
                /* make sure you can open the file */
                if (stat(gbuf.gl_pathv[i], &pstat) < 0) {
                    report_err("stat", gbuf.gl_pathv[i]);
                    continue;
                }
                typematches = filter_type(opts->type_arg, pstat.st_mode);
            }
            if (typematches) {
                printf("%s/%s\n", currpath, gbuf.gl_pathv[i]);
            }
        }
    }
    (*num_glob_call)++;
}

/* switch on user selected type,
 * if file type matches selected type, return non zero
 * this file matches
 */
int filter_type(char type, mode_t mode) {
    int matches = EXIT_FAILURE;
    switch (type) {
        case 'f':
            matches = S_ISREG(mode);
            break;
        case 'd':
            matches = S_ISDIR(mode);
            break;
        case 'b':
            matches = S_ISBLK(mode);
            break;
        case 'c':
            matches = S_ISCHR(mode);
            break;
        case 'p':
            matches = S_ISFIFO(mode);
            break;
        case 'l':
            matches = S_ISLNK(mode);
            break;
        case 's':
            matches = S_ISSOCK(mode);
            break;
        default:
            report_err("filter_type", &type);
    }
    return matches;
}

