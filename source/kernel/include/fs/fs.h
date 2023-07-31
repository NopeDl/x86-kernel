#ifndef FS_H
#define FS_H

int sys_fopen(const char* filename, int flags, ...);
int sys_fread(int file, char* ptr, int len);
int sys_fwrite(int file, char* ptr, int len);
int sys_lseek(int file, int ptr, int dir);
int sys_fclose(int file);

#endif