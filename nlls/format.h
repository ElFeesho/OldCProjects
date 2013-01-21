#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <time.h>

extern char *fname_format(char *name, char *root, int mode, int link);
extern char *mode_format(int mode);
extern char *time_format(time_t *mod_time);
extern char *size_format(unsigned long long size);
extern char *user_format(int uid);
extern char *group_format(int gid);
extern char *nested_format(char *fname);

#endif
