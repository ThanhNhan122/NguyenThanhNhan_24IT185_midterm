#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ls.h"

void list_directory(const char *path, const Options *opts, int print_dirname) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        perror(path);
        return;
    }

    if (!S_ISDIR(st.st_mode) || opts->dir_only) {
        FileEntry fe;
        memset(&fe, 0, sizeof(fe));
        strncpy(fe.name, path, sizeof(fe.name) - 1);
        strncpy(fe.fullpath, path, sizeof(fe.fullpath) - 1);
        fe.st = st;
        display_entry(&fe, opts);
        return;
    }

    DIR *dir = opendir(path);
    if (!dir) {
        perror(path);
        return;
    }

    if (print_dirname) {
        printf("%s:\n", path);
    }

    int capacity = 64;
    int count = 0;
    FileEntry *entries = malloc(capacity * sizeof(FileEntry));
    if (!entries) {
        closedir(dir);
        perror("malloc");
        return;
    }

    long long total_blocks = 0;
    struct dirent *dp;

    while ((dp = readdir(dir)) != NULL) {
        if (!opts->show_all) {
            if (opts->almost_all) {
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) continue;
            } else if (dp->d_name[0] == '.') {
                continue;
            }
        }

        if (count >= capacity) {
            capacity *= 2;
            FileEntry *tmp = realloc(entries, capacity * sizeof(FileEntry));
            if (!tmp) {
                free(entries);
                closedir(dir);
                perror("realloc");
                return;
            }
            entries = tmp;
        }

        memset(&entries[count], 0, sizeof(FileEntry));
        strncpy(entries[count].name, dp->d_name, sizeof(entries[count].name) - 1);

        if (strcmp(path, "/") == 0) {
            snprintf(entries[count].fullpath, sizeof(entries[count].fullpath), "/%s", dp->d_name);
        } else {
            snprintf(entries[count].fullpath, sizeof(entries[count].fullpath), "%s/%s", path, dp->d_name);
        }

        if (lstat(entries[count].fullpath, &entries[count].st) == 0) {
            total_blocks += entries[count].st.st_blocks;
            count++;
        }
    }
    closedir(dir);

    if (opts->long_format || opts->blocks) {
        if (opts->kilobytes) total_blocks = (total_blocks + 1) / 2;
        printf("total %lld\n", total_blocks);
    }

    sort_files(entries, count, opts);

    for (int i = 0; i < count; i++) {
        display_entry(&entries[i], opts);
    }

    if (opts->recursive) {
        for (int i = 0; i < count; i++) {
            if (S_ISDIR(entries[i].st.st_mode) &&
                strcmp(entries[i].name, ".") != 0 &&
                strcmp(entries[i].name, "..") != 0) {
                putchar('\n');
                list_directory(entries[i].fullpath, opts, 1);
            }
        }
    }

    free(entries);
}
