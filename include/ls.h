#ifndef LS_H
#define LS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

typedef enum {
    TIME_MODIFIED = 0,
    TIME_STATUS_CHANGE,
    TIME_ACCESS
} TimeType;

typedef struct {
    int show_all;        /* -a */
    int almost_all;      /* -A */
    int dir_only;        /* -d */
    int recursive;       /* -R */
    int long_format;     /* -l */
    int numeric_id;      /* -n */
    int inode;           /* -i */
    int blocks;          /* -s */
    int human_readable;  /* -h */
    int kilobytes;       /* -k */
    int classify;        /* -F */
    int no_sort;         /* -f */
    int reverse_sort;    /* -r */
    int sort_size;       /* -S */
    int sort_time;       /* -t */
    int raw_chars;       /* -w */
    int hide_nonprint;   /* -q */
    TimeType time_type;  /* -c hoac -u */
} Options;

typedef struct {
    char name[512];
    char fullpath[1024];
    struct stat st;
} FileEntry;

void list_directory(const char *path, const Options *opts, int print_dirname);
void display_entry(const FileEntry *entry, const Options *opts);
void sort_files(FileEntry *entries, int count, const Options *opts);

#endif
