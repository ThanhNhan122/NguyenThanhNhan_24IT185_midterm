#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ls.h"

int main(int argc, char *argv[]) {
    Options opts;
    memset(&opts, 0, sizeof(Options));
    opts.hide_nonprint = isatty(STDOUT_FILENO);

    int opt;
    while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (opt) {
            case 'A': opts.almost_all = 1; break;
            case 'a': opts.show_all = 1; break;
            case 'c': opts.time_type = TIME_STATUS_CHANGE; break;
            case 'd': opts.dir_only = 1; opts.recursive = 0; break;
            case 'F': opts.classify = 1; break;
            case 'f': opts.no_sort = 1; opts.show_all = 1; break;
            case 'h': opts.human_readable = 1; opts.kilobytes = 0; break;
            case 'i': opts.inode = 1; break;
            case 'k': opts.kilobytes = 1; opts.human_readable = 0; break;
            case 'l': opts.long_format = 1; opts.numeric_id = 0; break;
            case 'n': opts.long_format = 1; opts.numeric_id = 1; break;
            case 'q': opts.hide_nonprint = 1; opts.raw_chars = 0; break;
            case 'R': opts.recursive = 1; opts.dir_only = 0; break;
            case 'r': opts.reverse_sort = 1; break;
            case 'S': opts.sort_size = 1; break;
            case 's': opts.blocks = 1; break;
            case 't': opts.sort_time = 1; break;
            case 'u': opts.time_type = TIME_ACCESS; break;
            case 'w': opts.raw_chars = 1; opts.hide_nonprint = 0; break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    int remaining = argc - optind;
    if (remaining == 0) {
        list_directory(".", &opts, 0);
    } else if (remaining == 1) {
        list_directory(argv[optind], &opts, 0);
    } else {
        FileEntry files[512];
        char *dirs[512];
        int file_count = 0, dir_count = 0;

        for (int i = optind; i < argc; i++) {
            struct stat st;
            if (lstat(argv[i], &st) != 0) {
                perror(argv[i]);
                continue;
            }
            if (S_ISDIR(st.st_mode) && !opts.dir_only) {
                dirs[dir_count++] = argv[i];
            } else {
                strncpy(files[file_count].name, argv[i], sizeof(files[file_count].name) - 1);
                files[file_count].name[sizeof(files[file_count].name) - 1] = '\0';
                strncpy(files[file_count].fullpath, argv[i], sizeof(files[file_count].fullpath) - 1);
                files[file_count].fullpath[sizeof(files[file_count].fullpath) - 1] = '\0';
                files[file_count].st = st;
                file_count++;
            }
        }

        if (file_count > 0) {
            sort_files(files, file_count, &opts);
            for (int i = 0; i < file_count; i++) {
                display_entry(&files[i], &opts);
            }
        }

        for (int i = 0; i < dir_count; i++) {
            if (file_count > 0 || i > 0) putchar('\n');
            list_directory(dirs[i], &opts, 1);
        }
    }
    return 0;
}
