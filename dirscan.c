// ===============================================
// File:         read.cxx
// Description:	 
// Version 1:    
// Current:      26 Sep 1999
// Author(s):    Thomas Kern
// ===============================================

//

/*
 * Copyright (C) 1997-1998  Thomas Kern
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 */


//=====================================================================
//	INCLUDES
//=====================================================================
//
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"


#ifndef UNIX
// For some reason this does not work in linux.  Investigate...
#define USE_DIRENT 1
#endif

#ifdef _WIN32_NATIVE_FILE_OPS
 #include "windows.h"
#endif
#ifdef USE_DIRENT
#if defined(MAC)
#include "macos_dirent.h"
#elif defined(WINDOWS)
#include "win32_dirent.h"
#else
#include <dirent.h>  // directory operations
#endif
#else
 #include <sys/types.h>
 #include <dirent.h>  // directory operations
 #include <values.h>

 #include <glob.h>
#endif

#include "dirscan.h"

//=====================================================================
// Why are we recreating scandir?
//=====================================================================
#if 0
int
scandir(const char *dirname, struct dirent ***namelist,
	int (*select)(struct dirent *),
	int (*compar)(const void *, const void *));

int
alphasort(const void *d1, const void *d2);

EXAMPLE
      The example program below scans the /tmp directory.  It does not
      exclude any entries since select is NULL.  The contents of namelist
      are sorted by alphasort().  It prints out how many entries are in /tmp
      and the sorted entries of the /tmp directory.  The memory used by
      scandir() is returned using free().


extern int scandir();
extern int alphasort();

main()
{
  int num_entries, i;
  struct dirent **namelist, **list;
  
  if ((num_entries = scandir("/tmp", &namelist, NULL, alphasort)) < 0) {
    fprintf(stderr, "Unexpected error\n");
    exit(1);
  }
  printf("Number of entries is %d\n", num_entries);
  if (num_entries) {
    printf("Entries are:");
    for (i=0, list=namelist; i<num_entries; i++) {
      printf(" %s", (*list)->d_name);
      free(*list);
      *list++;
    }
    free(namelist);
    printf("\n");
  }
  printf("\n");
  exit(0);
}



      #include <regex.h>

      int regcomp(regex_t *preg, const char *pattern, int cflags);

      int regexec(
           const regex_t *preg,
           const char *string,
           size_t nmatch,
           regmatch_t pmatch[],
           int eflags
      );

      void regfree(regex_t *preg);

      size_t regerror(
           int errcode,
           const regex_t *preg,
           char *errbuf,
           size_t errbuf_size
      );


 EXAMPLES
           /* match string against the extended regular expression in pattern,
           treating errors as no match.  Return 1 for match, 0 for no match.
           Print an error message if an error occurs. */

           int
           match(string, pattern)
           char *string;
           char *pattern;
           {
               int i;
               regex_t re;
               char buf[256];

               i=regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB);
               if (i != 0) {
                   (void)regerror(i,&re,buf,sizeof buf);
                   printf("%s\n",buf);
                   return(0);                       /* report error */
               }
               i = regexec(&re, string, (size_t) 0, NULL, 0);
               regfree(&re);
               if (i != 0) {
                   (void)regerror(i,&re,buf,sizeof buf);
                   printf("%s\n",buf);
                   return(0);                       /* report error */
               }
               return(1);
           }



#include <stdlib.h>
#include <string.h>
#include <glob.h>

/* Convert a wildcard pattern into a list of blank-separated
   filenames which match the wildcard.  */

char * glob_pattern(char *wildcard)
{
  char *gfilename;
  size_t cnt, length;
  glob_t glob_results;
  char **p;

  glob(wildcard, GLOB_NOCHECK, 0, &glob_results);

  /* How much space do we need?  */
  for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc;
       cnt; p++, cnt--)
    length += strlen(*p) + 1;

  /* Allocate the space and generate the list.  */
  gfilename = (char *) calloc(length, sizeof(char));
  for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc;
       cnt; p++, cnt--)
    {
      strcat(gfilename, *p);
      if (cnt > 1)
        strcat(gfilename, " ");
    }

  globfree(&glob_results);
  return gfilename;
}


include <glob.h>

glob_t result;

glob ("~homer/bin/*", GLOB_TILDE, NULL, &result);
globfree (result);


#include <glob.h>
#include <stdio.h>
#include "dinstall.h"

int no_match(const char *path, const char *name, __mode_t mode){
/* [<][>][^][v][top][bottom][index][help] */
  int i,status;
  glob_t globbuf;
  struct stat statbuf;
  snprintf(prtbuf,PRTBUFSIZE,"%s/*/%s",path,name);
  glob( T_FILE(prtbuf), GLOB_NOSORT, NULL, &globbuf);
  status=1;
  if ( globbuf.gl_pathc > 0 ) {
    for (i=0;i<globbuf.gl_pathc;i++) {
      if ( (! stat(globbuf.gl_pathv[i],&statbuf) ) &&
           ( (statbuf.st_mode & S_IFMT) == mode ) )  {
        status=0;
        break;
      }
    }
  }
  globfree(&globbuf); 
  return status;
}


#endif

//=====================================================================
static const int maxDir = 10;
static char scannedDir[10][NAMELENGTH];
static int  dirCount = 0;

//---------------------------------------------------------------------
// ein Verzeichnis unter auslesen
int ScanDirectory(char *dir, char *pattern, int firstfile,
                   char  filelist[MAX_DIR_ENTRIES][NAMELENGTH])
{
  char  wildcard[NAMELENGTH];
  int   lenDir;
  int   filecount = 0;
  int   i;
#ifdef _WIN32_NATIVE_FILE_OPS
  HANDLE            searchPath = NULL;
  // LPCTSTR == const char *
  WIN32_FIND_DATA   dataStruct;
#endif
#ifdef USE_DIRENT
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
#else // Unix
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
  int            test, lenf, lenp;
  char          *file;

  char *gfilename;
  size_t cnt, length;
  glob_t glob_results;
  char **p;
#endif

  for(i = 0; i < MAX_DIR_ENTRIES; i++)
    strcpy(filelist[i], "");

  concat_path(dir, pattern, wildcard);

#ifdef _WIN32_NATIVE_FILE_OPS
  searchPath = FindFirstFile( (LPCTSTR)wildcard, &dataStruct);
  if( searchPath == INVALID_HANDLE_VALUE ) 
  {
    printf("INVALID_HANDLE_VALUE\n");
    return 0;
  }

  // Skip files before our current window.
  for ( filecount = 0; filecount < firstfile; )
  { 
    concat_path(dir, dataStruct.cFileName, filelist[0]);
    if (isDir(filelist[0]))
      strcpy(filelist[0], "");
    else
      filecount++;
    if (FindNextFile(searchPath, &dataStruct) == 0)
    {
      FindClose(searchPath);
      return 0;
    }
  }
  filecount = 0;
  while( filecount < MAX_DIR_ENTRIES )
  {  
    concat_path(dir, dataStruct.cFileName, filelist[filecount]);
    if (isDir(filelist[filecount]))
      strcpy(filelist[filecount], "");
    else
      filecount++;
    if (FindNextFile(searchPath, &dataStruct) == 0)
      break;
  }
  FindClose(searchPath);

#endif
#ifdef USE_DIRENT
  if( (searchPath = opendir(wildcard)) ) { 
    // Skip files before our current window.
    for ( filecount = 0; filecount < firstfile; )
    { 
      if (!(dataStruct = readdir(searchPath)))
      {
	closedir(searchPath);
	return(0);
      }
      concat_path(dir, dataStruct->d_name, filelist[0]);
      if (isDir(filelist[0]))
	strcpy(filelist[0], "");
      else
	filecount++;
    }
    filecount = 0;
    while( filecount < MAX_DIR_ENTRIES
	   && (dataStruct = readdir(searchPath)) ) {
      concat_path(dir, dataStruct->d_name, filelist[filecount]);
      if (isDir(filelist[0]))
	strcpy(filelist[0], "");
      else
	filecount++;
    }
    closedir(searchPath);
  }
#else // Unix
  glob(wildcard, 0, 0, &glob_results);

  // Skip files before our current window.
  filecount = 0;
  for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc;
       cnt; p++, cnt--)
  {
    if ( filecount >= firstfile )
      break;
    if (!isDir(*p))
      filecount++;
  }

  /* generate the list.  */
  for ( filecount = 0; cnt; p++, cnt--)
  {
    if (filecount >= MAX_DIR_ENTRIES)
      break;
    if (!isDir(*p))
    {
      strcpy(filelist[filecount], *p);
      filecount++;
    }
  }
  globfree(&glob_results);

#endif 

  return filecount;
}

//---------------------------------------------------------------------
// ein Verzeichnis unter auslesen
int ScanFolder(char *dir, char *pattern, int firstfile,
                   char  filelist[MAX_DIR_ENTRIES][NAMELENGTH])
{
  char  wildcard[NAMELENGTH];
  int   lenDir;
  int   filecount = 0;
  int   i;
#ifdef _WIN32_NATIVE_FILE_OPS
  HANDLE            searchPath = NULL;
  // LPCTSTR == const char *
  WIN32_FIND_DATA   dataStruct;
#endif
#ifdef USE_DIRENT
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
#else // Unix
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
  int            test, lenf, lenp;
  char          *file;

  char *gfilename;
  size_t cnt, length;
  glob_t glob_results;
  char **p;
#endif
    
  for(i = 0; i < MAX_DIR_ENTRIES; i++)
    strcpy(filelist[i], "");

  concat_path(dir, pattern, wildcard);

#ifdef _WIN32_NATIVE_FILE_OPS
  searchPath = FindFirstFile( (LPCTSTR)wildcard, &dataStruct);
  if( searchPath == INVALID_HANDLE_VALUE ) 
  {
    printf("INVALID_HANDLE_VALUE\n");
    return 0;
  }

  // Skip files before our current window.
  for (filecount = 0; filecount < firstfile; )
  {
    concat_path(dir, dataStruct.cFileName, filelist[0]);
    if (isDir(filelist[0]))
      filecount++;
    else
      strcpy(filelist[0], "");
    if (FindNextFile(searchPath, &dataStruct) == 0)
    {
      FindClose(searchPath);
      return 0;
    }
  }

  filecount = 0;
  while( filecount < MAX_DIR_ENTRIES )
  {
    concat_path(dir, dataStruct.cFileName, filelist[filecount]);
    if (isDir(filelist[filecount]))
      filecount++;
    else
      strcpy(filelist[filecount], "");
    if (FindNextFile(searchPath, &dataStruct) == 0)
      break;
  }
  FindClose(searchPath);

#endif
#ifdef USE_DIRENT
  if( (searchPath = opendir(wildcard)) ) { 
    // Skip files before our current window.
    for ( filecount = 0; filecount < firstfile; )
    { 
      if (!(dataStruct = readdir(searchPath)))
      {
	closedir(searchPath);
	return(0);
      }
      concat_path(dir, dataStruct->d_name, filelist[filecount]);
      if (isDir(filelist[filecount]))
	filecount++;
      else
	strcpy(filelist[filecount], "");
    }
    filecount = 0;
    while( filecount < MAX_DIR_ENTRIES 
	   && (dataStruct = readdir(searchPath)) ) {
      concat_path(dir, dataStruct->d_name, filelist[filecount]);
      if (isDir(filelist[filecount]))
	filecount++;
      else
	strcpy(filelist[filecount], "");
    }
    closedir(searchPath);
  }
#else // Unix
  glob(wildcard, GLOB_PERIOD, 0, &glob_results);

  // Skip files before our current window.
  filecount = 0;
  for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc;
       cnt; p++, cnt--)
  {
    if ( filecount >= firstfile )
      break;
    if (isDir(*p))
      filecount++;
  }

  /* generate the list.  */
  for ( filecount = 0; cnt; p++, cnt--)
  {
    if (filecount >= MAX_DIR_ENTRIES)
      break;
    if (isDir(*p))
    {
      strcpy(filelist[filecount], *p);
      filecount++;
    }
  }
  globfree(&glob_results);

#endif 

  return filecount;
}

//---------------------------------------------------------------------
// see if its a directory
int isDir(char *filename)
{
  struct stat st;

  if (stat(filename, &st))
    return 0;

  if (S_ISDIR(st.st_mode))
    return 1;

  return 0;
}

//---------------------------------------------------------------------
// search for a file
boolean CheckFile(char *file, char *modus)
{
  FILE    *Input = NULL;
  boolean  found = false;
  char     fileName[NAMELENGTH];
  char     searchFile[NAMELENGTH];
  int      i;

  strcpy(fileName, basename(file));
  i = 0;
  while( (Input == NULL) && (i < dirCount) ) {
    searchFile[0] = '\0'; // String loeschen
    strcat(searchFile, scannedDir[i]);
    strcat(searchFile, fileName);
    strcpy(file, searchFile);
    Input = fopen(file, modus);
    i++;
  }
  if( Input == NULL ) {              // nicht in Unterverzeichnissen
    strcpy(file, fileName);
    Input = fopen(file, modus);
  }
    
  // letztmalig ueberprufen 
  if(Input) {
    fclose(Input);
    found = true;
  }
  return found;
}

