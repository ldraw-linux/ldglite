/*
 * Platform specific code for ldglite                          
 *                                                             
 * Concerned with:                                             
 *    pathname handling     
 *    non-standard library functions                                   
*/



#ifndef PLATFORM_H
#define PLATFORM_H

#include <string.h>
#include <stdlib.h>


// Flags indicating what functions are lacking on this platform

#if defined(UNIX) 

// unistd.h declares chdir() on Mac, Unix?
#include <unistd.h>

#define LACKS_STRDUP 1
#define LACKS_STRICMP 1
#define LACKS_STRNICMP 1
#define LACKS_DIRNAME 0
#define LACKS_BASENAME 0


#elif defined(MAC)

// unistd.h declares chdir() on Mac, Unix?
#include <unistd.h>

#define LACKS_STRDUP 1
#define LACKS_STRICMP 1
#define LACKS_STRNICMP 1
#define LACKS_DIRNAME 1
#define LACKS_BASENAME 1

#else  // Windows MS VC++

// NOTE: Gotta differentiate between VC++ and MINGW32 (or Cygnus).

#define LACKS_STRDUP 0
#define LACKS_STRICMP 0
#define LACKS_STRNICMP 0
#define LACKS_DIRNAME 1
#define LACKS_BASENAME 1

// Use convenient flag WINDOWS to look consistent
#ifndef WINDOWS
#define WINDOWS 1
#endif

#endif


// Declarations as needed

#if LACKS_STRDUP
char * strdup( const char *str );
#endif

#if LACKS_STRICMP
int stricmp( const char *str1, const char *str2 );
#endif

#if LACKS_STRNICMP
int strnicmp( const char *str1, const char *str2, size_t n );
#endif

#if LACKS_DIRNAME
char * dirname( const char *filepath );
#endif

#if LACKS_BASENAME
char * basename( const char *filepath );
#endif

// Plain Declarations

char *concat_path(const char *path1, const char *path2, char *result);
char *localize_path(char *inoutPath);
char *platform_getenv(const char *var);
void platform_comment(char *message, int level);
int platform_startup(int *argcp, char ***argv);


#endif // PLATFORM_H


