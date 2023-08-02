#ifndef FS_H
#define FS_H

#include <sys/stat.h>


int sys_fopen(const char* filename, int flags, ...);
int sys_fread(int file, char* ptr, int len);
int sys_fwrite(int file, char* ptr, int len);
int sys_lseek(int file, int ptr, int dir);
int sys_fclose(int file);

int sys_isatty(int file);
int sys_fstat(int file, struct stat* st);


#endif