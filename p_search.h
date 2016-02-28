
/* Project: pfind
 * Author:  Beth Boose
 * File:    p_search.h
 * Purpose: header file for p_search
 */

#ifndef P_SEARCH_H
#define P_SEARCH_H

#include <sys/stat.h>
/* struct to hold parsed validated user input
 * char *start_path directory/file name
 * int name_flag    do we have a name switch?
 * int type_flag    do we have a type switch?
 * char *name_arg   dir or file name/pattern to search for
 * char type_arg    file type to search for
 */
struct opt_info {
    char *start_path;
    int name_flag;
    int type_flag;
    char *name_arg;
    char type_arg;
};
void search_dir( char *parentdir, char * dirname, struct opt_info * opts );
void report_err (char *func_name, char *func_param );
void report_func_err(char *func_param);
void filter_results(struct opt_info * opts, char *currpath, int *num_glob_call);
int filter_type(char type, mode_t mode);
#endif /* P_SEARCH_H */

