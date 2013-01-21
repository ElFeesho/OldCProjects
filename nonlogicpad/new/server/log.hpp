#ifndef __LOG_H__
#define __LOG_H__

extern bool log_open();
extern void log_write(char *text);
extern void log_close();
#endif
