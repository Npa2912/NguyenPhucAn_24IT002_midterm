#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "listing.h"
#include "sorting.h"
#include "display.h"
#ifdef _WIN32
#define lstat stat
#endif

static char *duplicate(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}
static char *join(const char *dir, const char *name)
{
    size_t a = strlen(dir), b = strlen(name);
    char *p;
    if (a > (size_t)-1 - b - 2) return NULL;
    p = malloc(a + b + 2);
    if (p) { memcpy(p, dir, a); p[a] = '/'; memcpy(p+a+1, name, b+1); }
    return p;
}
static void free_entries(FileEntry *a, size_t n)
{
    size_t i;
    for (i=0; i<n; ++i) { free(a[i].name); free(a[i].path); }
    free(a);
}
static int append(FileEntry **a, size_t *n, size_t *cap, FileEntry e)
{
    FileEntry *tmp;
    size_t next;
    if (*n == *cap) {
        next = *cap ? *cap * 2 : 16;
        if (next < *cap || next > (size_t)-1 / sizeof(**a)) return -1;
        tmp = realloc(*a, next * sizeof(**a));
        if (!tmp) return -1;
        *a = tmp; *cap = next;
    }
    (*a)[(*n)++] = e;
    return 0;
}
static int visible(const char *name, const Options *o)
{
    if (o->show_all) return 1;
    if (!strcmp(name,".") || !strcmp(name,"..")) return 0;
    return o->almost_all || name[0] != '.';
}
static int list_dir(const char *path, const Options *o, int header)
{
    DIR *d;
    struct dirent *de;
    FileEntry *a = NULL;
    size_t n=0, cap=0, i;
    int status=0;
    d=opendir(path);
    if (!d) { fprintf(stderr,"myls: %s: %s\n",path,strerror(errno)); return 1; }
    while (1) {
        errno=0;
        de=readdir(d);
        if (de==NULL) {
            if (errno) {fprintf(stderr,"myls: %s: %s\n",path,strerror(errno));status=1;}
            break;
        }
        FileEntry e;
        if (!visible(de->d_name,o)) continue;
        e.name=duplicate(de->d_name);
        e.path=join(path,de->d_name);
        if (!e.name || !e.path) {
            free(e.name);free(e.path);status=1;break;
        }
        if (lstat(e.path,&e.info) != 0) {
            fprintf(stderr,"myls: %s: %s\n",e.path,strerror(errno));
            free(e.name);free(e.path);status=1;continue;
        }
        if (append(&a,&n,&cap,e) != 0) {
            free(e.name);free(e.path);status=1;break;
        }
    }
    if (closedir(d) != 0) status=1;
    if (status && n==0) { free_entries(a,n);return 1; }
    sort_entries(a,n,o);
    if (header) printf("%s:\n",path);
    display_total(a,n,o);
    for (i=0;i<n;i++) display_entry(&a[i],o);
    if (o->recursive) {
        for (i=0;i<n;i++) {
            if (S_ISDIR(a[i].info.st_mode) && strcmp(a[i].name,".") && strcmp(a[i].name,"..")) {
                putchar('\n');
                if (list_dir(a[i].path,o,1)) status=1;
            }
        }
    }
    free_entries(a,n);
    return status;
}
int list_operands(int count, char **paths, const Options *o)
{
    FileEntry *files=NULL,*dirs=NULL;
    size_t nf=0,nd=0,cf=0,cd=0,i;
    int status=0, printed=0;
    if (!count) { char *default_path="."; return list_operands(1,&default_path,o); }
    for (i=0;i<(size_t)count;i++) {
        FileEntry e;
        int is_dir;
        e.name=duplicate(paths[i]);e.path=duplicate(paths[i]);
        if (!e.name || !e.path) {free(e.name);free(e.path);status=1;continue;}
        if (lstat(e.path,&e.info)) {
            fprintf(stderr,"myls: %s: %s\n",e.path,strerror(errno));
            free(e.name);free(e.path);status=1;continue;
        }
        is_dir=S_ISDIR(e.info.st_mode);
#ifndef _WIN32
        if (!o->directory_only && S_ISLNK(e.info.st_mode)) {
            struct stat target;
            if (stat(e.path,&target)==0 && S_ISDIR(target.st_mode)) {
                e.info=target;is_dir=1;
            }
        }
#endif
        if (is_dir && !o->directory_only) {
            if (append(&dirs,&nd,&cd,e)) {free(e.name);free(e.path);status=1;}
        } else {
            if (append(&files,&nf,&cf,e)) {free(e.name);free(e.path);status=1;}
        }
    }
    sort_entries(files,nf,o);
    sort_entries(dirs,nd,o);
    for (i=0;i<nf;i++) {display_entry(&files[i],o);printed=1;}
    for (i=0;i<nd;i++) {
        if (printed) putchar('\n');
        if (list_dir(dirs[i].path,o,count>1 || o->recursive)) status=1;
        printed=1;
    }
    free_entries(files,nf);free_entries(dirs,nd);
    return status;
}
int list_path(const char *path, const Options *o)
{
    char *p=(char *)path;
    return list_operands(1,&p,o);
}
