#!/bin/sh

# change "-l3" in the line below LDGLITEARGS to the desired command line options
# change the path in the line below LDRAWDIR to the ldraw directory path

mkdir  ~/.MacOSX

if test ! -f ~/.MacOSX/environment.plist
then
echo "Creating environment.plist"
cat <<END > ~/.MacOSX/environment.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>LDGLITEARGS</key>
	<string>-l3</string>
	<key>LDRAWDIR</key>
	<string>/Library/ldraw</string>
</dict>
</plist>
END

else

echo "Checking for existing LDRAWDIR/LDGLITEARGS in environment.plist"
grep -q -e 'LDRAWDIR\|LDGLITEARGS' ~/.MacOSX/environment.plist

if [ $? == 0 ]; 
then
echo "LDGLITEARGS and(/or) LDRAWDIR are set, exiting"

else
echo "Adding LDGLITEARGS and LDRAWDIR to environment.plist"
# make backup for safety
cp ~/.MacOSX/environment.plist ~/.MacOSX/environment.backup
cat <<FIN > ~/.MacOSX/environment.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>LDGLITEARGS</key>
	<string>-l3</string>
	<key>LDRAWDIR</key>
	<string>/Library/ldraw</string>
FIN
tail +5 ~/.MacOSX/environment.backup | cat >> ~/.MacOSX/environment.plist
fi   #existing LDRAWDIR/LDGLITEARGS test

fi   #existing environment.plist test

