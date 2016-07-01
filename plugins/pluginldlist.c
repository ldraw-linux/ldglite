// ldlist requires the LDRAWDIR environment variable to find parts.
// It doesn't seem to use the ldraw.ini file

// I need to launch it and set it free so I can return from the fn.


#ifdef _WIN32
#include <windows.h>
#endif
#include <string.h>
#include <stdlib.h>

typedef void WINAPI (TLDDPCallBack) (char *strCBCompleteText, char *strCBSelText);

extern  void WINAPI __declspec(dllexport) Plugin_Info(unsigned char CaseID, char *buffer, unsigned char maxlength)

// This is the first function called by LDDesignPad.
// It contains info about the author of the plugin
// like name, description etc.

{
  char result[100];
  int length;

  switch (CaseID)
  {
    case 0: default: strcpy(result, "LDList"); //Name
                     break;

    case 1: strcpy(result, "LDList"); //Menu entry
            break;

    case 2: strcpy(result, "1.0"); //Version
            break;

    case 3: strcpy(result, "LDraw Part search"); //Description
            break;

    case 4: strcpy(result, "Anders Isaksson"); //Author
            break;

    case 5: strcpy(result, "Plugin wrapper adapted by DMH."); //Comment
            break;

    case 6: strcpy(result, "0"); //Plugin type

            // Plugintype can be one of the following values
            // 0 = Plugin can be called always
            // 1 = Plugin can only be called if no text is selected
            // 2 = Plugin can only be called if a portion of text is selected
            break;
  };

  //copy the string to the result buffer, but truncate it if it's to long.
  length=strlen(result)+1;
  if (length>maxlength)
  {
    length=maxlength;
    result[length-1]='\0';
    strncpy(buffer, result, length);
  }
  else 
    strcpy(buffer, result);

};


/**********************************************************************/
BOOL RegGetKeyStringValue(HKEY hMainKey, char szSubKey[256], char szValueName[], char szBuffer[], DWORD BufferSize)
{
  /* this will hold the handle to the key we will open */
  HKEY hOpenedKey;

  /* open the key with read access */
  if (!RegOpenKeyEx(hMainKey, szSubKey, 0, KEY_ALL_ACCESS, &hOpenedKey) == ERROR_SUCCESS)
  {
    /* we failed to open the key, return false */
    return FALSE;
  }
  
  /* get the value of the key */
  if (!RegQueryValueEx(hOpenedKey, szValueName, NULL, NULL, (unsigned char *)szBuffer, &BufferSize) == ERROR_SUCCESS)
  {
    /* we failed getting the value */
    return FALSE;
  }

  /* when we are finished, close the key */
  RegCloseKey(hOpenedKey);

  /* no errors, return true */
  return TRUE;
}

// ----------------------------------------------------------------------------
int launch()
{
  char progname[256];
  char *ptr;
  BOOL bRetVal;
  STARTUPINFO si;
  SECURITY_ATTRIBUTES saProcess, saThread;
  PROCESS_INFORMATION piProcess;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  saProcess.nLength = sizeof(saProcess);
  saProcess.lpSecurityDescriptor = NULL;
  saProcess.bInheritHandle = TRUE;
  saThread.nLength = sizeof(saThread);
  saThread.lpSecurityDescriptor = NULL;
  saThread.bInheritHandle = FALSE;

  //Gotta get the plugin directory the program was run in
  //So I can find ldlist.  This could be a problem in linux
  //unless I pass the path as an arg.  (no access to argv here)
  //Same goes for LDRAWDIR path.  Also for the windows ldlist
  //I need to putenv("LDRAWDIR=somewere") if it's not set already

  ptr = getenv("LDRAWDIR");

  if (!ptr || !strlen(ptr))
  {
    // Check ldraw.ini before going for LDDP registry setting?
    char SERVICE_KEY[256];
    char ldrawdir[256];
    
    strcpy(SERVICE_KEY, "Software\\Waterproof Productions\\LDDesignPad\\TfrOptions");

    if (GetPrivateProfileString("LDraw","BaseDirectory","",
				ldrawdir,256,"ldraw.ini") == 0)

      bRetVal = RegGetKeyStringValue(HKEY_CURRENT_USER, SERVICE_KEY, "edLDrawDir_Text", ldrawdir, 256);
    
    if (strlen(ldrawdir))
    {
      char ldrawdirenv[256];
      sprintf(ldrawdirenv, "LDRAWDIR=");
      strcat(ldrawdirenv, ldrawdir);
      putenv(ldrawdirenv);
    }
  }

  GetModuleFileName(NULL, progname, 256);
  // ptr = dirname(progname);
  if ( (ptr = strrchr(progname, '\\')) || (ptr = strrchr(progname, '/')) )
    *ptr = 0;
  strcat(progname, "\\plugins\\ldlist.exe");

  bRetVal = CreateProcess(progname, // App name
			  NULL, // Command line (for passing args)
			  &saProcess, // Process attributes
			  &saThread, // Thread attributes
			  FALSE, // Inherit handles
			  0, // Creation flags
			  NULL, // Use parent env
			  NULL, // Current directory
			  &si, // Startup info
			  &piProcess); // process info

  return (int)(bRetVal);
}

// ----------------------------------------------------------------------------
extern void WINAPI __declspec(dllexport) ProcessText(unsigned char *CompleteText, unsigned char *SelText, unsigned long *SelStart, unsigned long *SelLength, unsigned long *CursoRow, unsigned long *CursorColum, TLDDPCallBack myCallback)

//
// This function processes and Returns 'TheText'.
//
// SelLength: length of selection
// SelStart : start of selection IF the user selected
//           a portion of text. If the user didnt select any text then TheText
//           contains the complete text and SelLength is 0.
// Row , column: Actual cursor position.
//
// You can manipulate SelStart, Sellength, cursorrow, cursorcolumn
// by assigning new values to them.

{
  // EXAMPLE FUNCTION Start ***********************************************
    //Start the core of this little plugins function

  // Note: If strSeltext is NOT empty then only the selected text in LDDP
  //       will be replaced after returning from the plugin.
  //
  // Selstart,
  // Sellength: If Selstart and Sellength are 0 then LDDP is ignoring
  //            it and using the cursorRow, CursorCOlumn Settings instead


  // Launch a new process to run lddp
  launch();

  // EXAMPLE FUNCTION End ***********************************************

  // Leave the following line untouched!
  // This is the callback function to get the changed text back to LDDP

  myCallback(CompleteText, SelText);

  //release local copies of any strings.

};

int main()

{
  return 0;
};
