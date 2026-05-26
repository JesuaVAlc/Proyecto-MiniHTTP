#ifndef FILES_H
#define FILES_H

typedef struct {
    char path[4096];
    long size;
    int  fd;
} FileInfo;

int  openFile(const char *uri, const char *www_root, FileInfo *out);
void closeFile(FileInfo *f);

#endif