
// ***************************************************************************
//
//  macprefs.c 
//  routines to read, write, modify preferences on MacOS platform.
//  The preferences file is a text file where each line is of the form VAR=VALUE.
//  It also has a resource fork that stores 'alis' records.
//
// ***************************************************************************

#include <Aliases.h>
#include <Controls.h>
#include <Errors.h>
#include <Files.h>
#include <Folders.h>
#include <Resources.h>
#include <Scrap.h>
#include <Script.h>
#include <StandardFile.h>
#include <TextUtils.h>
#include <ToolUtils.h>

#include "MoreFilesExtras.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "platform.h"
#include "macprefs.h"

#include <Types.h>

// Globals
const OSType myCreatorType = 'LDGl';
const OSType myPrefType = 'pref';


const int MAX_MACPREFS = 10; // number of env vars to keep in MacPrefs

char **MacPrefs;
AliasHandle Alias_LDRAWDIR;
int MacPrefs_initialized;

// \p indicates a Pascal string of our filename (first byte is length byte)
Str255 PrefsFilename = "\pMacLDGLite Preferences";

static FSSpec MacPrefs_FileSpec;

const short kResID_Alias_LDRAWDIR = 607;

// Internal prototypes

int macprefs_OpenPrefsFile(FSSpec * specPtr, int writing, short *refNumPtr);
int macprefs_setenv_from_FSSpec(const char *envvar, FSSpec *specPtr);


// ***************************************************************************
//  macprefs_init - initialize globals, and load the prefs file if it exists
//                  returns 0 on failure, 1 if ok
// ***************************************************************************

int macprefs_init()
{
	int i;
	MacPrefs = NULL;
	MacPrefs_initialized = 0;
	
	MacPrefs = (char**)malloc(sizeof(char*) * MAX_MACPREFS);

	if (!MacPrefs) { return 0; }
	
	for (i = 0; i< MAX_MACPREFS; i++)
	{
		MacPrefs[i] = NULL;
	}
	MacPrefs_initialized = 1;
	
	if ( macprefs_load() == 0)
	{
		return macprefs_create();
	}

	return 1;	
}

// ***************************************************************************
//  macprefs_create - make a new prefs file - choose dir - make alias resource
//                  returns 0 on failure, 1 if ok
// ***************************************************************************
int macprefs_create()
{
	int rc;
	
	rc = macprefs_choosedir("LDRAW", "LDRAWDIR");
	
	if (rc != 0)
	{
		rc = macprefs_save();
	}

	return rc;
}

// ***************************************************************************
//  macprefs_getenv - return an item from already loaded prefs
//                  returns NULL on failure (no such var or not initialized)
// ***************************************************************************

char *macprefs_getenv(const char *var)
{
	int i, n;

	// On first call to macprefs_getenv, the Prefs file will be loaded
	if (!MacPrefs_initialized) 
	{ 
		if (macprefs_init() == 0)
		{
			return NULL;
		}
	}

	// Search MacPrefs array
	n = strlen(var);
	
	for (i = 0; i< MAX_MACPREFS && MacPrefs[i] != NULL; i++)
	{
		if (!strnicmp(MacPrefs[i], var, n) && MacPrefs[i][n] == '=')
		{
			// got match, return pointer to after the = sign
			return MacPrefs[i] + n + 1;
		}
	}
	return NULL;
}



// ***************************************************************************
//  macprefs_setenv - store an item in prefs
//                  returns 0 on failure (too many?)
// ***************************************************************************

int macprefs_setenv(const char *var, const char *value)
{
	int i, n;

	n = strlen(var);
	
	// Search MacPrefs array
	if (!MacPrefs_initialized) { return 0; }
	for (i = 0; i< MAX_MACPREFS && MacPrefs[i] != NULL; i++)
	{
		if (!strnicmp(MacPrefs[i], var, n) && MacPrefs[i][n] == '=')
		{
			// got match, overwrite this i
			break;
		}
	}
	if (i >= MAX_MACPREFS) { return 0; } // array full already, sorry
	// Free old string
	if (MacPrefs[i] != NULL) { free (MacPrefs[i]); MacPrefs[i] = NULL; }
	// Make new string
	MacPrefs[i] = (char*)malloc(strlen(var) + strlen(value) + 2);
	sprintf(MacPrefs[i], "%s=%s", var, value);
	return 1;
}


// ***************************************************************************
//  macprefs_load - load the prefs file if it exists
//                  returns 0 on failure, 1 if ok
// ***************************************************************************

int macprefs_load()
{
	// See if prefs file exists
	
	int i, n;
	long count;
	char *buf, *ptr, *ptr2;
	FSSpec Spec;
	OSErr theErr;
	short refNum, resRefNum;
	FInfo fndrInfo;
	Boolean	wasChanged;
	
	// Read existing prefs file
	if (! macprefs_OpenPrefsFile(&Spec, 0, &refNum))
	{
		return 0;
	}
	// Allocate space & slurp in whole file contents
	theErr = GetEOF( refNum, &count );
	if (theErr != noErr)
	{
		return 0;
	}
	
	buf = (char*) malloc(count + 2); // extra char in case missing end of line
	if (buf == NULL) 
	{
		return 0;
	}
	
	if (count != 0)
	{
		theErr = FSRead( refNum, &count, buf);
		if (theErr != noErr)
		{
			free(buf);
			return 0;
		}
	}
	
	buf[count] = 0;
	buf[count+1] = 0;
	
	i = 0;
	ptr = buf;
	
	while( (i< MAX_MACPREFS) && *ptr != 0 )
	{
		ptr2 = strchr(ptr, '\n');
		if (!ptr2) 
		{
			ptr2 = strchr(ptr, '\r');
		}
		if (!ptr2)
		{
			// missing end of line at end of file?
			ptr2 = ptr + strlen(ptr);
		}
		// Only store lines that have = signs (anything else is lost when we write it out again)
		if (strchr(buf, '='))
		{
			// Remove newline
			*ptr2 = 0;
			// Keep string as var=value
			MacPrefs[i] = strdup(ptr);
			i++;
		}
		ptr = ptr2 + 1;
	}

	if (i< MAX_MACPREFS)
	{
		MacPrefs[i] = NULL;
	}
	
	free (buf);
	FSClose(refNum);

	// Load From Resource Fork
	if (!macprefs_readwrite_alias(&Spec, 0))
	{
		return 0;
	}

	
	// Alias checking
	if (Alias_LDRAWDIR != NULL)
	{
		// Check that it is good
		theErr = ResolveAlias(NULL, Alias_LDRAWDIR, &Spec, &wasChanged);
		if (theErr == noErr && wasChanged)
		{
			// User has moved the LDRAW directory since last we ran, but we found it
			// Overwrite any text that we loaded
			macprefs_setenv_from_FSSpec("LDRAWDIR", &Spec);
			// Not necessary to write that out -- we'll find it any time.
		}
		else
		{
			
		}
	}
	else
	{
		theErr = fnfErr; // made-up error code.
	}

	
	
	// In these conditions we should query for where the folder went.
	// This will re-write the prefs as a side effect.
	if (theErr != noErr)
	{
		macprefs_choosedir("LDRAW", "LDRAWDIR");
	}
	
	return 1;
}

// ***************************************************************************
//  macprefs_save - write changed or new prefs file
//                  returns 0 on failure, 1 if ok
// ***************************************************************************

int macprefs_save()
{
	// Write prefs file

	int i, n;
	long count;
	char *buf, *ptr;
	FSSpec Spec;
	OSErr theErr;
	short refNum;
	Handle tempHandle;
	short resRefNum;
	
	// Prepare to make 1 big block out of MacPrefs strings
	n = 0;
	for (i = 0; i< MAX_MACPREFS && MacPrefs[i] != NULL; i++)
	{
		n += strlen(MacPrefs[i]) + 1;
	}
	n++;

	buf = (char*)malloc(n);
	if (buf == NULL) { return 0; }
	
	// Cat all MacPrefs strings into buf
	ptr = buf;
	for (i = 0; i< MAX_MACPREFS && MacPrefs[i] != NULL; i++)
	{
		strcpy(ptr, MacPrefs[i]);
		ptr += strlen(ptr);
		*ptr++ = '\n';
	}

	// Write buf to file
	
	if (! macprefs_OpenPrefsFile(&Spec, 1, &refNum))
	{
		free (buf);
		return 0;
	}
	count = n;
	theErr = FSWrite( refNum, &count, buf );
	FSClose(refNum);
	free(buf);

	if (theErr != noErr)
	{	
		return 0;
	}

	// Put alias in Resource Fork
	if (!macprefs_readwrite_alias(&Spec, 1))
	{
		return 0;
	}

	return 1;
}

// ************************************************************************************
//  macprefs_readwrite_alias - read or write the LDRAWDIR alias to prefs resource fork
// ************************************************************************************
int macprefs_readwrite_alias(FSSpec *specPtr, int writing)
{
	int permission;
	short resRefNum;
	OSErr theErr;
	
	permission = writing ? fsRdWrPerm : fsRdPerm;

	resRefNum = FSpOpenResFile(specPtr, permission );
	theErr = ResError();
	if (theErr == eofErr)
	{
		// Res fork is empty - Create resource map now
		FSpCreateResFile(specPtr, myCreatorType, myPrefType, smSystemScript);
		theErr = ResError();
		// Try again
		if (theErr == noErr)
		{
			resRefNum = FSpOpenResFile(specPtr, permission );
			theErr = ResError();
		}
	}
	if (resRefNum == -1 || theErr != noErr)
	{
		return 0;
	}

	// Res fork is open and ready to use (current)

	if (writing)
	{
		AddResource(Alias_LDRAWDIR, 'alis', kResID_Alias_LDRAWDIR, "\pLDRAWDIR");
		theErr = ResError();
		// this keeps a copy for us, and bequeaths the original to the Res Manager
		HandToHand(&Alias_LDRAWDIR); 

		// Close Resource Fork
	}	
	else
	{
		// Read the alias
		Alias_LDRAWDIR = Get1Resource('alis', kResID_Alias_LDRAWDIR);
		theErr = ResError();
		
		if (Alias_LDRAWDIR != NULL)
		{
			// Take ownership of resource
			DetachResource(Alias_LDRAWDIR);
		}
	}
	// Close Resource Fork
	CloseResFile(resRefNum);

	return (theErr == noErr) ? 1 : 0;
}


// ************************************************************************************
//  macprefs_choosedir - ask user to locate a needed folder & add it to prefs (setenv)
//   This depends on the MoreFiles package from Apple's Tool Chest CD
// ************************************************************************************
#include "StandardGetFolder.h"
#include "FullPath.h"

int macprefs_choosedir(const char *foldername, const char *envvar)
{
	FileFilterYDUPP 	customFilterUPP;
	StandardFileReply	mySFReply;
	
	OSErr				theErr;
	
	// Dialog...
	// It would be nice to customize the PROMPT to use foldername!!!
	
	customFilterUPP = NewFileFilterYDProc( OnlyVisibleFoldersCustomFileFilter );

	StandardGetFolder( customFilterUPP, &mySFReply );
	
	DisposeRoutineDescriptor( customFilterUPP );
	
	// Store the full path as a environment string
	macprefs_setenv_from_FSSpec(envvar, &mySFReply.sfFile);
	
	
	// Also Create an alias record
	theErr = NewAlias(NULL, &mySFReply.sfFile, &Alias_LDRAWDIR);
	if (theErr != noErr)
	{
		return 0;
	}
		
	return macprefs_save();
}


int macprefs_setenv_from_FSSpec(const char *envvar, FSSpec *specPtr)
{
	OSErr				theErr;
	short 				fullPathLength;
	Handle	 			fullPath;
	char 				chosenFolder[256];

	// Create a string
	theErr = FSpGetFullPath(specPtr, &fullPathLength, &fullPath);
	if (theErr != noErr)
	{
		return 0;
	}
	// Now make it a C string
	BlockMove(*fullPath, &chosenFolder[0], fullPathLength);
	chosenFolder[fullPathLength] = 0;
	DisposeHandle(fullPath);
	
	return macprefs_setenv(envvar, &chosenFolder[0]);

}


// *******************************************************************************
//  macprefs_OpenPrefsFile - find the prefs file, create it as needed if writing.
//                           Use the System Folder's Preferences folder.
//                           Then open the file for reading or writing.
// *******************************************************************************
int macprefs_OpenPrefsFile(FSSpec * specPtr, int writing, short *refNumPtr)
{
	OSErr theErr;
	short vref;
	long dirID;
	char permission;
	short resRefNum;
	
	if (specPtr == NULL) { return 0; }
	
	// Find the Preferences folder
	// we are NOT checking OS version - assume something modern (if it has OpenGL...)
	theErr = FindFolder( kOnSystemDisk, kPreferencesFolderType, true, &vref, &dirID );
	if (theErr != noErr) { return 0; }
	
	// Specify the prefs file spec within that folder
	theErr = FSMakeFSSpec( vref, dirID, PrefsFilename, specPtr );
	if (theErr == fnfErr )
	{
		// file not found, yet
		if (writing)
		{
			theErr = FSpCreate(specPtr, myCreatorType, myPrefType, smSystemScript );
			if (theErr != noErr)
			{
				return 0;
			}
			// It's good to populate the res fork now (suppose we crash before we get to it)
			FSpCreateResFile(specPtr, myCreatorType, myPrefType, smSystemScript);
			theErr = ResError();
			if (theErr != noErr)
			{
				return 0;
			}
		}
		else 
		{
			// reading, but file not found. Bad.
			return 0;
		}
	}
	else if (theErr != noErr)
	{
		return 0;
	}
	
	// Open the file
	permission = writing ? fsRdWrPerm : fsRdPerm;
	theErr = FSpOpenDF( specPtr, permission, refNumPtr );
	if (theErr != noErr)
	{
		return 0;
	}

	// File is open and ready to use.	
	return 1;
}
