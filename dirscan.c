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
#include <string.h>

#include "platform.h"

#ifdef _WIN32
 #include "windows.h"
#else
 #include <dirent.h>  // directory operations
 #include <values.h>
#endif

#include "dirscan.h"

//=====================================================================
// Dateien in verschiedenen Verzeichnissen suchen
//=====================================================================
//
static const int maxDir = 10;
static char scannedDir[10][NAMELENGTH]; // speichert die bisher genutzten Verzeichnisse
static int  dirCount = 0;

//---------------------------------------------------------------------
// ein Verzeichnis unter auslesen
int ScanDirectory(char *dir, char *pattern, int firstfile,
                   char  filelist[MAX_DIR_ENTRIES][NAMELENGTH])
{
  char  wildcard[NAMELENGTH];
  int   lenDir;
  int   filecount;
  int   i;
#ifdef _WIN32
  HANDLE            searchPath = NULL;
  // LPCTSTR == const char *
  WIN32_FIND_DATA   dataStruct;
#else // Unix
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
  int            test, lenf, lenp;
  char          *file;
#endif
    
#define DEBUG 1;

#ifdef DEBUG
    printf("searching %s for %s at %d\n", dir, pattern, firstfile);
#endif

  for(i = 0; i < MAX_DIR_ENTRIES; i++)
    strcpy(filelist[i], "");

  concat_path(dir, pattern, wildcard);

#ifdef DEBUG
    printf("wildcard = %s [%s]\n", dir, wildcard);
#endif

#if 1
  printf("finding files [%s] from %d \n", wildcard, firstfile);
#endif

  // -----------------------
  // nun wird's OS-abhaengig
#ifdef _WIN32

  searchPath = FindFirstFile( (LPCTSTR)wildcard, &dataStruct);
  if( searchPath == INVALID_HANDLE_VALUE ) 
  {
    printf("INVALID_HANDLE_VALUE\n");
    return 0;
  }

  // Skip files before our current window.
  for ( filecount = 0; filecount < firstfile; )
  { 
    concat_path(dir, dataStruct.cFileName, filelist[filecount]);
    if (isDir(filelist[filecount]))
      strcpy(filelist[filecount], "");
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
#ifdef DEBUG
    printf("finding %s - %s\n", dir, dataStruct.cFileName);
#endif
    if (isDir(filelist[filecount]))
      strcpy(filelist[filecount], "");
    else
      filecount++;
    if (FindNextFile(searchPath, &dataStruct) == 0)
      break;
  }
  FindClose(searchPath);

#ifdef DEBUG
  printf("found %d files\n", filecount);
#endif

#else // Unix
  // Verzeichnis oeffnen
  if( (searchPath = opendir(dir)) ) { 
    // Skip files before our current window.
    for ( filecount = 0; filecount < firstfile; filecount++ )
    { 
      if (!dataStruct = readdir(searchPath);)
      {
	closedir(searchPath);
	return(0);
      }
    }
    filecount = 0;
    while(    filecount < MAX_DIR_ENTRIES // Grenzen beachten
	   && (dataStruct = readdir(searchPath)) ) {
      file = dataStruct->d_name;
      lenf = strlen(file);      
      lenp = strlen(pattern);
      test = 0;
      for(i = 1; i < 4; i++) {
	      if( file[lenf-i] == pattern[lenp-i] )
	        test++;
      }
      if( test == 3 ) { // Dateiendung stimmt ueberein
	concat_path(dir, dataStruct->d_name, filelist[filecount]);
	filecount++;
      }
    }
    closedir(searchPath);
  }
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
  int   filecount;
  int   i;
#ifdef _WIN32
  HANDLE            searchPath = NULL;
  // LPCTSTR == const char *
  WIN32_FIND_DATA   dataStruct;
#else // Unix
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
  int            test, lenf, lenp;
  char          *file;
#endif
    
  for(i = 0; i < MAX_DIR_ENTRIES; i++)
    strcpy(filelist[i], "");

  concat_path(dir, pattern, wildcard);

#ifdef DEBUG
  printf("wildcard = %s [%s]\n", dir, wildcard);
#endif

#if 1
  printf("finding folders [%s] from %d \n", wildcard, firstfile);
#endif

  // -----------------------
  // nun wird's OS-abhaengig
#ifdef _WIN32

  searchPath = FindFirstFile( (LPCTSTR)wildcard, &dataStruct);
  if( searchPath == INVALID_HANDLE_VALUE ) 
  {
    printf("INVALID_HANDLE_VALUE\n");
    return 0;
  }

  // Skip files before our current window.
  for (filecount = 0; filecount < firstfile; )
  {
    concat_path(dir, dataStruct.cFileName, filelist[filecount]);
    if (isDir(filelist[filecount]))
    {
#if 1
      printf("skipping dir %d %s - %s\n", filecount, dir, dataStruct.cFileName);
#endif
      filecount++;
    }
    else
    {
#if 1
      printf("skipping %d %s - %s\n", filecount, dir, dataStruct.cFileName);
#endif
      strcpy(filelist[filecount], "");
    }
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
#if 1
    printf("checking %s - %s\n", dir, dataStruct.cFileName);
#endif
    if (isDir(filelist[filecount]))
    {
      filecount++;
#if 1
      printf("found dir %s - %s\n", dir, dataStruct.cFileName);
#endif
    }
    else
      strcpy(filelist[filecount], "");
    if (FindNextFile(searchPath, &dataStruct) == 0)
      break;
  }
  FindClose(searchPath);

#if 1
  printf("found %d folders\n", filecount);
#endif

#else // Unix
  // Verzeichnis oeffnen
  if( (searchPath = opendir(dir)) ) { 
    // Skip files before our current window.
    for ( filecount = 0; filecount < firstfile; filecount++ )
    { 
      if (!dataStruct = readdir(searchPath);)
      {
	closedir(searchPath);
	return(0);
      }
    }
    filecount = 0;
    while(    filecount < MAX_DIR_ENTRIES // Grenzen beachten
	   && (dataStruct = readdir(searchPath)) ) {
      file = dataStruct->d_name;
      lenf = strlen(file);      
      lenp = strlen(pattern);
      test = 0;
      for(i = 1; i < 4; i++) {
	      if( file[lenf-i] == pattern[lenp-i] )
	        test++;
      }
      if( test == 3 ) { // Dateiendung stimmt ueberein
	concat_path(dir, dataStruct->d_name, filelist[filecount]);
	filecount++;
      }
    }
    closedir(searchPath);
  }
#endif 

  return filecount;
}

//---------------------------------------------------------------------
// Verzeichnis und Dateiname trennen
static void extractDir(char *complete, char *directory, char *file)
{
  char    tempName[NAMELENGTH];
  int     len, i, j;
  boolean haveDir = false;

  strcpy(directory, complete);
  len = strlen(directory);

  // falls kein Vezeichnis angegeben, den Rest ueberspringen
  for(i = 0; i < len; i++) {
    if( directory[i] == '/' ) {
      haveDir = true;
      break;
    }
  }

  if(!haveDir) {
    directory[0] = '\0';
    strcpy(file, complete);
  }
  else {
    // Verzeichnis und Dateinamen trennen
    i = len-1; 
    j = 0;
    while( i && directory[i] != '/' ) {
      tempName[j]  = directory[i];  // vor dem Loeschen noch speichern
      j++;
      directory[i] = '\0';
      i--;
    }

    // Dateiname muss noch in die richtige Reihenfolge gebracht werden
    tempName[j] = '\0'; // String zuende
    len = strlen(tempName);
    for(i = 0; i < len; i++) 
      file[i] = tempName[len-i-1];
    file[i] = '\0';
  
    // Verzeichnis fuer spaeteres Durchsuchen speichern
    if(!dirCount) { // erstmaliger Aufruf
      strcpy(scannedDir[dirCount], directory);
      dirCount++;
    }
    else if(dirCount < maxDir) {
      i = 0;
      j = 0;  
      while(i < dirCount) {
        j += strcmp(scannedDir[i], directory); // Verzeichnis schon bekannt ?
        i++;
      }
      if(j) {                                  // Antwort ist: nein
        strcpy(scannedDir[dirCount], directory);
        dirCount++;
      }
    }
  }
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
  char     dirName[NAMELENGTH], fileName[NAMELENGTH];
  char     searchFile[NAMELENGTH];
  int      i;

  extractDir(file, dirName, fileName);
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

#ifdef testing
//=====================================================================
// Main Function: reads subdirectories for *.wrl files
//=====================================================================
//
int main(int argc, char **argv)
{  
  int         i, len1;
  const int   len3 = WORDLENGTH;
  char        myDir[len3+1];
  char        pattern[] = {"*.wrl"};
  int         DateiCount    = 0;
  char        DateiListe[MAX_DIR_ENTRIES][NAMELENGTH];
  char        StandardDir[] = {"VRML/"};


  // Einstellmoeglichkeiten per Tastatur
  printf("\n======================================\n");
  printf("This prog shows how to scan a directory with various OS\n");
  printf("======================================\n");
  fflush(stdout);

  // Verzeichnisnamen ermitteln
  if(argc > 2 && !strcmp(argv[1],"-dir") ) {
    len1 = strlen(argv[1]);
    if( argv[2][len1-1] != '/' )
      sprintf(myDir, "%s/", argv[2]);
    else
      sprintf(myDir, "%s", argv[2]);
  }
  else
    sprintf(myDir, "%s", StandardDir); // prog dir

  // Dateinamen ermitteln
  ScanDirectory(myDir, pattern, DateiListe, DateiCount);
  if(!DateiCount) { // Verzeichnis enthielt keine Dateien
    strcpy(myDir, "./");
    ScanDirectory(myDir, pattern, DateiListe, DateiCount);
  }

  // print the filenames
  printf("found %d files which matched %s-pattern:\n", DateiCount, pattern);
  for(i = 0; i < DateiCount; i++) {
    if( CheckFile(DateiListe[i], "r") ) {// file > 0 byte ??
      printf("file %d:\t %s\n", i+1, DateiListe[i]);
    }
  }

  return 0;
}

#endif 
