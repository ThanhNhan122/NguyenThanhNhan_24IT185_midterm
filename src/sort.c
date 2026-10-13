#include <string.h>
#include <stdlib.h>
#include "ls.h"

static const Options *current_opts;

static time_t get_file_time(const FileEntry *e, const Options *opts) {
    if (opts->time_type == TIME_STATUS_CHANGE) return e->st.st_ctime;
    if (opts->time_type == TIME_ACCESS) return e->st.st_atime;
    return e->st.st_mtime;
}

static int compare_entries(const void *a, const void *b) {
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;
    int res = 0;

    if (current_opts->sort_size) {
        if (fa->st.st_size > fb->st.st_size) res = -1;
        else if (fa->st.st_size < fb->st.st_size) res = 1;
        else res = strcmp(fa->name, fb->name);
    } else if (current_opts->sort_time) {
        time_t ta = get_file_time(fa, current_opts);
        time_t tb = get_file_time(fb, current_opts);
        if (ta > tb) res = -1;
        else if (ta < tb) res = 1;
        else res = strcmp(fa->name, fb->name);
    } else {
        res = strcmp(fa->name, fb->name);
    }

    return current_opts->reverse_sort ? -res : res;
}

void sort_files(FileEntry *entries, int count, const Options *opts) {
    if (opts->no_sort) return;
    current_opts = opts;
    qsort(entries, count, sizeof(FileEntry), compare_entries);
}
