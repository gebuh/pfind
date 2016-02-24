
/* Project: pfind
 * Author:  Beth Boose
 * File:    pfind.c
 * Purpose: allow user to search for files and directories by name and type
 */

#include "p_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

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

/* collect options for getopts */
static struct option longopts[] = {
    {"name", required_argument, NULL, 'n'},
    {"type", required_argument, NULL, 't'},
    {NULL, 0, NULL, 0}
};

/*
 * on error or call to -h, print pfind usage instructions
 */
static void print_usage() {
    printf("Usage: pfind START_DIRECTORY "
            "[-name FILENAME/PATTERN] [-type {f|d|b|c|p|l|s}]\n");
    printf("Options: \n");
    printf("-name string literal pattern to search for\n");
    printf("-type single character one of: f|d|b|c|p|l|s\n");
    printf("-h          print usage\n");
}

static int populate_optinfo(char *start_path,
        int name_flag,
        int type_flag,
        char *name_arg,
        char *type_arg,
        struct opt_info *optinfo);

static int validate_type_arg(char* type_arg);

/* validate type_arg length and against the allowable file types
 * if not return exit_failure
 */
int validate_type_arg(char* type_arg) {

    /* should only be one char */
    if ((strlen(optarg)) > 1) {
        printf("too many characters for -type\n");
        return EXIT_FAILURE;
    }

    const char allowable[] = "fdbcpls";
    int len = strspn(allowable, type_arg);
    /* only one of allowable chars */
    if (len != 1) {
        printf("%s is not an allowable character\n", type_arg);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*
 * 
 */
int main(int argc, char * argv[]) {
    int type_flag = 0; /* count number of -type args */
    int name_flag = 0; /* count number of -name args */

    char *start_path = NULL; /* placeholder for starting directory */
    char *name_arg = NULL;
    char *type_arg = NULL;

    int c = 0;
    int index;
    const char *optstr = "t:n:h?";

    while ((c = getopt_long_only(argc, argv, optstr, longopts, &index)) != -1) {
        switch (c) {
            case 'n':
                if ((name_flag += 1) > 1) {
                    printf("only 1 -name parameter allowed \n");
                    print_usage();

                }
                name_arg = optarg;
                break;
            case 't':
                if ((validate_type_arg(optarg)) != EXIT_SUCCESS ) {
                    printf("-type param incorrect \n");
                    print_usage();
                    exit(EXIT_FAILURE);
                }
                type_arg = optarg;
                break;
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            case '?':
                printf("you did something wrong\n");
                print_usage();
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= 1) {
        start_path = argv[1];
    }

    struct opt_info optinfo;
    memset ( &optinfo, 0, sizeof(optinfo) );
    populate_optinfo(start_path, name_flag, type_flag, 
        name_arg, type_arg, &optinfo);


    /* search for something */
    search_dir ( optinfo.start_path );
    return (EXIT_SUCCESS);

}

/* populate opt_info struct with 
 * 
 */
int populate_optinfo(char *start_path,
        int name_flag,
        int type_flag,
        char *name_arg,
        char *type_arg,
        struct opt_info *optinfo) {

    int isvalid = EXIT_SUCCESS;
    /* set correct directory */
    optinfo->start_path = start_path;
    if (start_path == NULL) {
        getcwd(optinfo->start_path, sizeof (optinfo->start_path));
    }

    

    /* populate the optinfo */
    optinfo->name_arg = start_path;
    optinfo->name_flag = name_flag;
    optinfo->type_flag = type_flag;
    optinfo->name_arg = name_arg;
    
    /*convert the string to a char */
    if (type_flag == 1) { 
        optinfo->type_arg = type_arg[0];
    }

    return isvalid;
}


