#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "display.h"
#include "utils.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#ifdef __linux__
#include <sys/sysmacros.h>
#endif
#endif
#ifdef _WIN32
#ifndef S_IXGRP
#define S_IXGRP 0010
#define S_IXOTH 0001
#endif
#endif
static char suffix(mode_t m)
{
    if (S_ISDIR(m)) return '/';
#ifdef S_ISLNK
    if (S_ISLNK(m)) return '@';
#endif
#ifdef S_ISWHT
    if (S_ISWHT(m)) return '%';
#endif
#ifdef S_ISSOCK
    if (S_ISSOCK(m)) return '=';
#endif
#ifdef S_ISFIFO
    if (S_ISFIFO(m)) return '|';
#endif
    if (S_ISREG(m) && (m & (S_IXUSR|S_IXGRP|S_IXOTH))) return '*';
    return 0;
}
static time_t file_time(const FileEntry *e, const Options *o)
{
    if (o->change_time) return e->info.st_ctime;
    if (o->access_time) return e->info.st_atime;
    return e->info.st_mtime;
}
static void print_name(const char *name, const Options *o)
{
    const unsigned char *s=(const unsigned char *)name;
    int escape=o->escape_names && !o->raw_names;
    for (;*s;s++) putchar(escape && !isprint(*s) ? '?' : *s);
}
static long long blocks512(const struct stat *st)
{
#ifdef _WIN32
    return ((long long)st->st_size+511)/512;
#else
    return (long long)st->st_blocks;
#endif
}
static long long rounded(long long blocks, const Options *o)
{
    long long bytes=blocks*512;
    long long unit=512;
    const char *env=getenv("BLOCKSIZE");
    if (o->kilobytes) unit=1024;
    else if (!o->human_readable && env && *env) {
        char *end;
        long value=strtol(env,&end,10);
        if (value>0 && (*end=='\0' || ((*end=='k'||*end=='K') && end[1]=='\0'))) {
            unit=(long long)value;
            if (*end) unit*=1024;
        }
    }
    return (bytes+unit-1)/unit;
}
void display_entry(const FileEntry *e, const Options *o)
{
    char perms[11],owner[128],group[128],size[64],date[64];
    char mark;
    if (o->show_inode) printf("%llu ",(unsigned long long)e->info.st_ino);
    if (o->show_blocks) {
        if (o->human_readable) {
            char human[64];format_size(blocks512(&e->info)*512,human,sizeof(human));
            printf("%s ",human);
        } else printf("%lld ",rounded(blocks512(&e->info),o));
    }
    if (o->long_format) {
        format_permissions(e->info.st_mode,perms);
        format_owner(&e->info,o->numeric_ids,owner,sizeof(owner));
        format_group(&e->info,o->numeric_ids,group,sizeof(group));
        if (o->human_readable) format_size((long long)e->info.st_size,size,sizeof(size));
        else snprintf(size,sizeof(size),"%lld",(long long)e->info.st_size);
#ifndef _WIN32
        if (S_ISCHR(e->info.st_mode) || S_ISBLK(e->info.st_mode))
            snprintf(size,sizeof(size),"%u, %u",(unsigned)major(e->info.st_rdev),(unsigned)minor(e->info.st_rdev));
#endif
        format_file_time(file_time(e,o),date,sizeof(date));
        printf("%s %lu %-10s %-10s %8s %s ",perms,
            (unsigned long)e->info.st_nlink,owner,group,size,date);
    }
    print_name(e->name,o);
    mark=o->classify?suffix(e->info.st_mode):0;
    if (mark) putchar(mark);
#ifndef _WIN32
    if (o->long_format && S_ISLNK(e->info.st_mode)) {
        char target[4096];
        ssize_t n=readlink(e->path,target,sizeof(target)-1);
        if (n>=0) {target[n]=0;printf(" -> ");print_name(target,o);}
    }
#endif
    putchar('\n');
}
void display_total(const FileEntry *a,size_t n,const Options *o)
{
    size_t i;long long blocks=0;
    if (!o->long_format && !o->show_blocks) return;
    for (i=0;i<n;i++) blocks+=blocks512(&a[i].info);
    if (o->human_readable) {
        char human[64];format_size(blocks*512,human,sizeof(human));
        printf("total %s\n",human);
    } else printf("total %lld\n",rounded(blocks,o));
}
