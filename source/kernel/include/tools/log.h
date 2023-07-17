#ifndef LOG_H
#define LOG_H

#define COM1_PORT   0x3F8

void log_init();
void log_printf(const char* msg, ...);

#endif