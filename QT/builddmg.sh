#! /bin/sh

echo Stripping executable...
strip LDView.app/Contents/MacOS/LDView
if [ ! -d distrib ]; then
	mkdir distrib
fi
if [ -d distrib/LDView ]; then
	rm -rf distrib/LDView
fi
mkdir distrib/LDView
FileVersion=`grep 'VALUE "FileVersion"' ../LDView.rc | cut -d, -f2 | cut -d\" -f2 | cut -d\\\\ -f1`
Filename="LDView $FileVersion Universal"
echo Copying files...
cp ../8464.mpd distrib/LDView/
cp ../ChangeHistory.html distrib/LDView
cp -R LDView.App distrib/LDView/
cp ../license.txt distrib/LDView
cp ../m6459.ldr distrib/LDView
echo Building DMG...
hdiutil create -ov -srcfolder distrib/LDView -volname "$Filename" -format UDBZ "distrib/$Filename.dmg"
