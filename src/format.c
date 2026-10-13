#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "ls.h"

static void format_mode(mode_t m, char *str) {
    if (S_ISDIR(m)) str[0] = 'd';
    else if (S_ISLNK(m)) str[0] = 'l';
    else if (S_ISCHR(m)) str[0] = 'c';
    else if (S_ISBLK(m)) str[0] = 'b';
    else if (S_ISFIFO(m)) str[0] = 'p';
    else if (S_ISSOCK(m)) str[0] = 's';
    else str[0] = '-';

    str[1] = (m & S_IRUSR) ? 'r' : '-';
    str[2] = (m & S_IWUSR) ? 'w' : '-';
    if (m & S_ISUID) str[3] = (m & S_IXUSR) ? 's' : 'S';
    else str[3] = (m & S_IXUSR) ? 'x' : '-';

    str[4] = (m & S_IRGRP) ? 'r' : '-';
    str[5] = (m & S_IWGRP) ? 'w' : '-';
    if (m & S_ISGID) str[6] = (m & S_IXGRP) ? 's' : 'S';
    else str[6] = (m & S_IXGRP) ? 'x' : '-';

    str[7] = (m & S_IROTH) ? 'r' : '-';
    str[8] = (m & S_IWOTH) ? 'w' : '-';
    if (m & S_ISVTX) str[9] = (m & S_IXOTH) ? 't' : 'T';
    else str[9] = (m & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

static void print_human_size(off_t size) {
    const char *units[] = {"B", "K", "M", "G", "T"};
    int i = 0;
    double d_size = (double)size;
    while (d_size >= 1024.0 && i < 4) {
        d_size /= 1024.0;
        i++;
    }
    if (i == 0) printf("%8lld ", (long long)size);
    else printf("%7.1f%s ", d_size, units[i]);
}

void display_entry(const FileEntry *entry, const Options *opts) {
    if (opts->inode) {
        printf("%llu ", (unsigned long long)entry->st.st_ino);
    }
    if (opts->blocks) {
        long long blks = (long long)entry->st.st_blocks;
        if (opts->kilobytes) blks = (blks + 1) / 2;
        printf("%lld ", blks);
    }

    if (opts->long_format) {
        char mode_str[11];
        format_mode(entry->st.st_mode, mode_str);

        struct passwd *pw = getpwuid(entry->st.st_uid);
        struct group *gr = getgrgid(entry->st.st_gid);

        printf("%s %2ld ", mode_str, (long)entry->st.st_nlink);

        if (!opts->numeric_id && pw != NULL) {
            printf("%-8s ", pw->pw_name);
        } else {
            printf("%-8u ", (unsigned)entry->st.st_uid);
        }

        if (!opts->numeric_id && gr != NULL) {
            printf("%-8s ", gr->gr_name);
        } else {
            printf("%-8u ", (unsigned)entry->st.st_gid);
        }

        if (opts->human_readable) {
            print_human_size(entry->st.st_size);
        } else if (opts->kilobytes) {
            printf("%8lldK ", (long long)((entry->st.st_size + 1023) / 1024));
        } else {
            printf("%8lld ", (long long)entry->st.st_size);
        }

        time_t ftime = (opts->time_type == TIME_STATUS_CHANGE) ? entry->st.st_ctime :
                       (opts->time_type == TIME_ACCESS) ? entry->st.st_atime : entry->st.st_mtime;
        struct tm *tm_info = localtime(&ftime);
        char time_buf[64] = {0};
        if (tm_info) {
            strftime(time_buf, sizeof(time_buf), "%b %e %H:%M", tm_info);
            printf("%s ", time_buf);
        } else {
            printf("            ");
        }
    }

    for (const char *p = entry->name; *p; p++) {
        if (opts->hide_nonprint && !isprint((unsigned char)*p)) putchar('?');
        else putchar(*p);
    }

    if (opts->classify) {
        mode_t m = entry->st.st_mode;
        if (S_ISDIR(m)) putchar('/');
        else if (S_ISLNK(m)) putchar('@');
        else if (S_ISSOCK(m)) putchar('=');
        else if (S_ISFIFO(m)) putchar('|');
        else if (m & (S_IXUSR | S_IXGRP | S_IXOTH)) putchar('*');
    }

    if (opts->long_format && S_ISLNK(entry->st.st_mode)) {
        char link_target[1024];
        ssize_t len = readlink(entry->fullpath, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }
    putchar('\n');
}
