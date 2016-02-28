
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

static void populate_optinfo(char *start_path,
        int name_flag,
        int type_flag,
        char *name_arg,
        char *type_arg,
        struct opt_info *optinfo);

static int validate_type_arg(char* type_arg);

/*
 * collect options, run the search
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
    opterr = 0; /* I want to use a custom message */

    while ((c = getopt_long_only(argc, argv, optstr, longopts, &index)) != -1) {
        switch (c) {
            case 'n':
                if ((name_flag += 1) > 1) {
                    exit(EXIT_FAILURE);
                }
                name_arg = optarg;
                break;
            case 't':
                if ((validate_type_arg(optarg)) != EXIT_SUCCESS) {
                    exit(EXIT_FAILURE);
                }
                if ((type_flag += 1) > 1) {
                    exit(EXIT_FAILURE);
                }
                type_arg = optarg;
                break;
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
            case '?':
                if (optopt) {
                    printf("pfind: missing argument to `%s'\n",
                            argv[optind - 1]);
                } else {
                    printf("pfind: unknown predicate `%s'\n",
                            argv[optind - 1]);
                } /* go ahead and fall thru */
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= 1) { /* get the start path if it exists */
        start_path = argv[optind];
    }

    if (start_path == NULL) { /* no start path provided? use current dir */
        start_path = ".";
    }

    struct opt_info optinfo;
    memset(&optinfo, 0, sizeof (optinfo));
    populate_optinfo(start_path, name_flag, type_flag,
            name_arg, type_arg, &optinfo);


    /* search for something */
    search_dir("", optinfo.start_path, &optinfo);
    return (EXIT_SUCCESS);

}

/* populate opt_info struct with 
 * user input
 */
void populate_optinfo(char *start_path,
        int name_flag,
        int type_flag,
        char *name_arg,
        char *type_arg,
        struct opt_info *optinfo) {

    /* set correct directory or fail */
    optinfo->start_path = start_path;


    /* if no name given use  * you'll thank me later */
    if (name_arg == NULL) {
        name_arg = "*";
    }

    /* populate the optinfo */
    optinfo->name_flag = name_flag;
    optinfo->type_flag = type_flag;
    optinfo->name_arg = name_arg;

    /*convert the string to a char */
    if (type_flag == 1) {
        optinfo->type_arg = type_arg[0];
    }
}

/* validate type_arg length and against the allowable file types
 * if not return exit_failure
 */
int validate_type_arg(char* type_arg) {
    /* should only be one char */
    if ((strlen(optarg)) > 1) {
        printf("Arguments to -type should contain only one letter\n");
        return EXIT_FAILURE;
    }

    const char allowable[] = "fdbcpls";
    int len = strspn(type_arg, allowable);
    /* only one of allowable chars */
    if (len != 1) {
        printf("Unknown argument to -type: %s\n", type_arg);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


