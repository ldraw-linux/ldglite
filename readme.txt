                                LdGLite
               (a platform independent version of LdLite)

LdLite is an LDRAW file viewer for Windows.  You can use it to view
DAT and MPD files containing models made with everyone's favorite 
construction bricks.

LdGLite is a platform independent version of LdLite.  It achieves this
by using the OpenGL graphics interface and the GLUT toolkit.  It 
currently runs in Windows, Linux, and MacOS.  It does not yet provide
all of the functionality of LdLite.  However it does have a few small
goodies not yet available in LdLite.

L3Glite is ldglite except it uses the L3 parser.  Actually the executables
are identical, the program decides which parser to use based on the name
of the program.  On linux you can softlink l3glite to ldglite.  You can
also force the program to use a specific parser with a -ld or -l3 command
line option.


                        LdGLite for Windows README

You must have a copy of opengl32.dll installed.  This comes with
Windows 95 OSR2, Windows 98, Windows 2000, and Windows NT.  If you use
an early version of Windows 95 then you must get opengl95.exe from
Microsoft and install it.

    ftp://ftp.microsoft.com/Softlib/MSLFILES/opengl95.exe

You must also install the GLUT dll in the appropriate directory.  For
Windows 95 and 98 copy glut32.dll to the windows\system directory.
For Windows NT and 2000 copy glut32.dll to the winnt\system32
directory.

You must also install the ldraw parts files from www.ldraw.org.

    c:
    cd \
    mkdir c:\ldraw
    cd c:\ldraw
    ldraw.exe -y
    complete.exe -y

For ldglite to find the parts you must set the environment variable
LDRAWDIR.  Add this line to your C:\AUTOEXEC.BAT file and reboot your
PC.  

    SET LDRAWDIR=C:\LDRAW

As of version 7.1 ldglite can also use the BaseDirectory setting in 
ldraw.ini to find the parts files.  The ldraw.ini file is created by
LDAO and various other ldraw type programs.

To use ldglite from MS Internet Explorer do this:

    1) Open an Explorer window (or any folder window).
    2) Select the "View->Folder Options" menu selection.
    3) Go to the "File Types" tab.
    4) Scroll down and double click on the "ldlite Document" line.  
       If there is no "ldlite Document" line, click the "New Type" button
       and enter "ldlite Document" in the "Description of type" field
       and ".dat" for the "Associated extension".
    5) In the "Content Type" field, type in "application/x-ldraw"
    6) In the "Actions" area, Click on the "New..." button.  
       For the Action name, type "open".  
       For the filename, browse to the location of ldglite.exe

For Netscape:

    1) Go to the Netscape menu item Edit/Preferences/Navigator/Applications.
    2) Select "ldlite Document".   
       If you don't find "ldlite Document" then add it.
    3) Edit, and set the mime type to application/x-ldraw.
    4) Browse for the ldglite.exe executable.

For Internet Explorer in Win2k (and perhaps Millenium):

    Thank you Micro$oft.  In Win2k you can no longer create new MIME types
    from an Explorer window.  You must use regedit instead.

    1) Start/Run
       Type "regedit" in the box and click "OK"

    2) Right click on HKEY_CLASSES_ROOT.
       Add a new key and rename it to ".dat"
       Right click on (Default) for this new key and modify the value.
       Enter "ldlite.Document" for the Value data.

    3) Right click on the key HKEY_CLASSES_ROOT/.dat
       Add a new string value and rename it to "Content Type"
       Right click on "Content Type" and select modify.
       Change the Value data to "application/x-ldraw".

    4) Find the key:
       HKEY_CLASSES_ROOT/MIME/Database/Content Type.  
       Under this key, create a key for application/x-ldraw.

    5) Under the application/x-ldraw key, create a string value:
       "Extension", with data ".dat".

    6) Quit regedit.

    7) Open an Explorer window (or any folder window).

    8) Select the "Tools->Folder Options" menu selection.

    9) Go to the "File Types" tab.

   10) Scroll down and double click on the "ldlite Document" line.  
       (On Win2K search for the "DAT" extension)
           If there is no "DAT" extension line, click the "New" button
           and then click on the "Advanced >>" button.
           Enter "DAT" in the "File Extension" field.
           and scroll to select "ldlite Document" for "Associated File Type".
           (If you can't find it go back to step 1)
           Press OK.

   11) Press the "Advanced" button.

   12) If "open" is not listed under "Actions" click the "New" button.
       Otherwise click on "open" and then click "Edit".
       
   13) In the "Actions" field, enter "open" if it's not already there.
       For "Application...", browse to the location of ldglite.exe (or l3glite.exe)

   14) Add any extra command line arguments in the "Application..." field
       Mine says this (I use the l3 parser and a small window):  
       C:\projects\ldglite\ldglite -l3 -v3 "%1"

   15) Press enough OK buttons to finish this up.



                        LdGLite for linux README

You must install the basic ldraw files from www.ldraw.org

    cd /usr/local
    mkdir ldraw
    cd ldraw
    unarj x ldraw.exe 
    unarj x complete.exe

The default directory for ldraw dat files is /usr/local/ldraw.
If you wish to install the files elsewhere, set the environment
variable LDRAWDIR

    LDRAWDIR=/var/ldraw
    export LDRAWDIR

The default case for dat files in the p and parts subdirectories
is lowercase.  If you want to use uppercase set the environment
variable LDRAWIRCASE

    LDRAWDIRCASE=UPPERCASE
    export LDRAWDIR

Unzip the ldglite distribution.  I zipped it with infozip which is free
on the web.  (I'm skipping the usual .tgz file since geocities seems to
choke on it.)  The Mandrake 6.1 linux distribution came with an unzip
utility preinstalled in /usr/bin. 

You must build ldglite

    make -f makefile.linux
    cp ldglite /usr/local/bin
    ln -s /usr/local/bin/ldglite /usr/local/bin/l3glite

Of course this assumes you have Mesa or some other OpenGL installed.
I had to make some softlinks for the Mesa libs on my Mandrake 6.1
linux distribution before the ldglite would link.

    cd /usr/X11R6/lib
    ln -s libMesaGLU.so libGLU.so
    ln -s libMesaGL.so libGL.so

I think the makefile also assumes you have libpng and libz installed.
Check makefile.linux for instructions on how to skip them if you have 
trouble linking.

For Netscape, go to the Netscape menu item
Edit/Preferences/Navigator/Applications, select "ldlite Document",
Edit, and set the mime type to application/x-ldraw.  If you don't find 
"ldlite Document" then add it.
Set the executable like this.  

    /usr/local/bin/ldglite %s

Feel free to substitute whatever path you use for the executable.  The
%s is required to pass the temporary filename of the dat file to
ldglite.  If you want a 640x480 window with shading turned on try something
like this.  See ldlite docs for help with command line options. 

    /usr/local/bin/ldglite -v3 -fh %s




                             Extra Stuff


Command line options (see also the original ldlite docs for more):

-fh turns on shading mode.
-fs turns off stud drawing. (l3glite only)
-v-1 displays in fullscreen with no decorations.
-v-2 displays in fullscreen gamemode (no menus in gamemode).
-x displays the x,y,z axis.
-iN picks output image type. 1 = PNG, 2 = Transparent PNG, 3 = BMP8, 4 = BMP24 
-i-N turns off cropping and picks image type.
-eN.NN fixes polyline offset driver errors.  Try -e0.95 if lines look bad.
-p turns on polling mode.
-ld forces the program to use the ldlite parser.
-l3 forces the program to use the l3 parser.
-wN sets linewidth to N.
-q quality lines (antialiased)
-& windows mode.  Detach from the console window.  Windows version only.

NOTE: if no DAT filename is given on the command line ldglite will read
from stdin.  (l3glite does not do this)  Try this:
  cat file.dat | ldglite -v3 -fh
Kinda neat, but I don't know if this is good for much.


Hot Keys (especially useful in fullscreen gamemode):

F10 enters fullscreen gamemode.
F9 exits fullscreen gamemode.
Right Mouse button displays the popup menu.
Left Mouse Drag will rotate the model.
Shift Mouse Drag rotates the model in solid bounding box mode.
Ctrl Mouse Drag rotates the model in solid studless mode.
Arrow keys turn the viewpoint camera 5 degrees left, right, up, down.
Alt Arrow keys roll the camera left, right, up, down.
Ctrl Arrow keys move the camera left, right, up, down.
PGUP and PGDN move the camera in and out (only visible in perspective mode)
0-9 keys pick one of the preset viewpoints.
s shrinks the model by half.
S scales the model up to double size.
z zooms out 10%.
Z sooms in 10%.
g toggles polling mode.
j picks the orthographic projection.
J picks the perspective projection.
l toggles the parser (LdLite or L3)
n turns on normal (no) shading.
h turns on shading.
w turns on wireframe mode.
f toggles stud drawing.
v toggles visible spin mode.
p prints a BMP file.
P prints a PNG file.
y attempts to scale the model to fit the window.  Needs work though.
q toggles line quality (antialiasing)
Esc quits.


