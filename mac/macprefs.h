#ifndef MACPREFS_H
#define MACPREFS_H

int macprefs_init();
int macprefs_load();
int macprefs_save();
char *macprefs_getenv(const char *var);
int macprefs_setenv(const char *var, const char *value);
int macprefs_choosedir(const char *foldername, const char *var);


#include <Types.h>

extern const OSType myCreatorType;
extern const OSType myPrefType;



#endif // MACPREFS_H
