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

Of course this assumes you have Mesa or some other OpenGL installed.
I had to make some softlinks for the Mesa libs on my Mandrake 6.1
linux distribution before the ldglite would link.

    cd /usr/X11R6/lib
    ln -s libMesaGLU.so libGLU.so
    ln -s libMesaGL.so libGL.so


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


