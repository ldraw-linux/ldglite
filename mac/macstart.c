
#include "macstartup.h"

#include <AppleEvents.h>
#include <Gestalt.h>
#include <Errors.h>
#include <Memory.h>
#include <Files.h>

#include "FullPath.h" // MoreFiles package

#include <console.h> // MSL SIOUX console library
#include <SIOUX.h> // MSL SIOUX console library

// Follows MacTech tutorial code for handling Apple Events:
// http://www.mactech.com/articles/mactech/Vol.10/10.12/RequiredEvents/


#define kGestaltMask 1L

typedef struct 
{
	int *argcp;
	char **argv;

} macStartupRecord;

macStartupRecord gStartupRefCon;

// Declarations
OSErr macStartupInstallAEHandlers(macStartupRecord *refCon);

OSErr CheckForRequiredParams( AppleEvent *event );
OSErr HandleOpenApplicationEvent(AppleEvent *event, AppleEvent *reply, long refcon);
OSErr HandleOpenDocumentEvent(AppleEvent *event, AppleEvent *reply, long refcon);
OSErr HandlePrintDocumentEvent(AppleEvent *event, AppleEvent *reply, long refcon);
OSErr HandleQuitApplicationEvent(AppleEvent *event, AppleEvent *reply, long refcon);
OSErr macFSpOpenDocument(FSSpec *specPtr);


int macStartup(int *argcp, char ***argv)
{

  #if 1
	// The Apple Events way
	gStartupRefCon.argcp = argcp;
	gStartupRefCon.argv  = argv;

	macStartupInstallAEHandlers(&gStartupRefCon);
	#else

  // The args dialog way
  	
  SIOUXSettings.asktosaveonclose = 0;
  
	*argcp = ccommand(argv);
	
  #endif
  
	return 0;
}


                  
OSErr macStartupInstallAEHandlers(macStartupRecord *refCon)
{
	AEEventHandlerUPP	Handler;
	short				numTables;
	int					i, j;
	OSErr				theErr;
  long        feature;

	theErr = noErr;

	// initialize Handler
  theErr = Gestalt( gestaltAppleEventsAttr, &feature );
	if (theErr == noErr && (feature  & ( kGestaltMask << gestaltAppleEventsPresent ) ))
	{
    // The system isn't prehistoric, it will provide the:
  	// Required Suite of high-level events
  	theErr = AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc( HandleOpenApplicationEvent ), (long)refCon, false);
  	theErr = AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerProc( HandleOpenDocumentEvent ), (long)refCon, false);
  	theErr = AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerProc( HandlePrintDocumentEvent ), (long)refCon, false);
  	theErr = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc( HandleQuitApplicationEvent ), NULL, false);
			
  }
	return theErr;
}

OSErr CheckForRequiredParams( AppleEvent *event )
{
     DescType returnedType;
     Size   actualSize;
     OSErr  err;

     err = AEGetAttributePtr( event, keyMissedKeywordAttr,
     typeWildCard, &returnedType,
     nil, 0, &actualSize);

     if ( err == errAEDescNotFound ) // you got all the required
     //parameters
     return noErr;
     else
     if ( err == noErr )      // you missed a required parameter
     return errAEParamMissed;
     else   // the call to AEGetAttributePtr failed
     return err;
}
    
OSErr HandleOpenApplicationEvent(AppleEvent *event, AppleEvent *reply, long refcon)
{
  OSErr  err = noErr;
	macStartupRecord *StartupRecord;
	StartupRecord = (macStartupRecord*)refcon;
	
	// We're opening the application with no documents coming:
	// Go into command line mode (ick)
	if (StartupRecord != NULL && StartupRecord->argv != NULL && StartupRecord->argcp != NULL)
	{
  	*StartupRecord->argcp = ccommand(StartupRecord->argv);
  	ParseParams(StartupRecord->argcp, *StartupRecord->argv);

  }
	
	return err;
	
}

OSErr HandleOpenDocumentEvent(AppleEvent *event, AppleEvent *reply, long refcon)
{
  OSErr  err = noErr;
  FSSpec fileSpec;
  long   i, numDocs;
  DescType returnedType;
  AEKeyword keywd;
  Size   actualSize;
  AEDescList docList = { typeNull, nil };

	macStartupRecord *StartupRecord = (macStartupRecord*)refcon;;
	
	
	
  // get the direct parameter--a descriptor list--and put
  // it into docList
  err = AEGetParamDesc( event, keyDirectObject, typeAEList, &docList);

  // check for missing required parameters
  err = CheckForRequiredParams( event );
  if ( err )
  {
    // an error occurred:  do the necessary error handling
    err = AEDisposeDesc( &docList );
    return err;
  }

  // count the number of descriptor records in the list
  // should be at least 1 since we got called and no error
  err = AECountItems( &docList, &numDocs );

  if ( err )
  {
    // an error occurred:  do the necessary error handling
    err = AEDisposeDesc( &docList );
    return err;
  }

  // now get each descriptor record from the list, coerce
  // the returned data to an FSSpec record, and open the
  // associated file
  for ( i=1; i<=numDocs; i++ )
  {
    err = AEGetNthPtr( &docList, i, typeFSS, &keywd, &returnedType, 
                       (Ptr)&fileSpec, sizeof( fileSpec ), &actualSize );

    macFSpOpenDocument( &fileSpec );
  }

  err = AEDisposeDesc( &docList );

  return err;
}

OSErr macFSpOpenDocument(FSSpec *specPtr)
{
  OSErr       theErr;
	short 			fullPathLength;
	Handle	 		fullPath;
	char 				fullPathString[256];

	// Create a string
	theErr = FSpGetFullPath(specPtr, &fullPathLength, &fullPath);
	if (theErr != noErr)
	{
		return theErr;
	}
	// Now make it a C string
	BlockMove(*fullPath, &fullPathString[0], fullPathLength);
	fullPathString[fullPathLength] = 0;
	DisposeHandle(fullPath);
	
	setfilename( &fullPathString[0]);
	
  return theErr;
	

}


OSErr HandlePrintDocumentEvent(AppleEvent *event, AppleEvent *reply, long refcon)
{
  OSErr  err = noErr;
	macStartupRecord *StartupRecord = (macStartupRecord*)refcon;;
	
  return err;
	
}

OSErr HandleQuitApplicationEvent(AppleEvent *event, AppleEvent *reply, long refcon)
{
  OSErr  err = noErr;
	macStartupRecord *StartupRecord = (macStartupRecord*)refcon;;
	
  return err;
	
}

