#!/bin/sh

rm -rf ldglite.app
mkdir -p ldglite.app/Contents
mkdir -p ldglite.app/Contents/MacOS
mkdir -p ldglite.app/Contents/Resources
cat <<END > ldglite.app/Contents/Info.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
<plist version="0.9">
  <dict>
    <key>CFBundleDevelopmentRegion</key>	<string>English</string>
    <key>CFBundleInfoDictionaryVersion</key>	<string>6.0</string>
    <key>CFBundleExecutable</key>		<string>ldglite</string>
    <key>CFBundlePackageType</key>		<string>APPL</string>
    <key>CFBundleSignature</key>		<string>LdGL</string>
    <key>CFBundleName</key>			<string>ldglite</string>
    <key>CFBundleVersion</key>			<string>1.3.1</string>
    <key>CFBundleShortVersionString</key>	<string>ldglite 1.3.1</string>
    <key>CFBundleGetInfoString</key>		<string>ldglite 1.3.1 http://ldglite.sourceforge.net</string>
    <key>CFBundleIconFile</key>			<string>ldglite.icns</string>
    <key>CFBundleIdentifier</key>		<string>org.ldraw.ldglite</string>
    <key>CSResourcesFileMapped</key>		<true/>
    <key>CFBundleDocumentTypes</key>
    <array>
      <dict>
	<key>CFBundleTypeExtensions</key>
          <array>
	    <string>LDR</string>
	    <string>ldr</string>
	    <string>MPD</string>
	    <string>mpd</string>
	    <string>DAT</string>
	    <string>dat</string>
          </array>
	<key>CFBundleTypeIconFile</key> 	<string>129</string>
	<key>CFBundleTypeName</key>		<string>Ldraw Document</string>
	<key>CFBundleTypeMIMETypes</key>    
	  <array>      
	    <string>application/x-ldraw</string>    
	  </array>    
	<key>CFBundleTypeOSTypes</key>		<array><string>LDR </string></array>
	<key>CFBundleTypeRole</key>		<string>Viewer</string>
	<key>NSDocumentClass</key>      
	  <string>BrowserDocument</string>
      </dict>
      <dict>
	<key>CFBundleTypeExtensions</key>	<array><string>bundle</string></array>
	<key>CFBundleTypeIconFile</key>		<string>ldglitePlugin.icns</string>
	<key>CFBundleTypeName</key>		<string>ldglite Plugin</string>
	<key>CFBundleTypeOSTypes</key>		<array><string>BNDL</string></array>
	<key>CFBundleTypeRole</key>		<string>Editor</string>
      </dict>
    </array>
  </dict>
</plist>
END
echo "APPLLdGL" > ldglite.app/Contents/PkgInfo

cp ldglite ldglite.app/Contents/MacOS
cp ldglite.icns ldglite.app/Contents/Resources
cat <<DONE > ldglite.app/Contents/MacOS/ldgliteWrapper.command
#!/bin/sh
pwd
ldglite -l3 -v4 $@
DONE
chmod 755 ldglite.app/Contents/MacOS/ldgliteWrapper.command

