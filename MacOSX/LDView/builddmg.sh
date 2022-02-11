#! /bin/sh
if [ -z "${1}" ]; then
	echo "Usage: builddmg.sh [directory]"
	exit 1
fi
if [ -d "${1}/Contents" ]; then
    app_root="${1}"
elif [ -d "${1}/Products/Users/${USER}/Applications/LDView.app/Contents" ]; then
    app_root="${1}/Products/Users/${USER}/Applications/LDView.app"
else
	echo Invalid directory: ${1}
	exit 1
fi
echo Creating Disk Image from ${app_root}...
if [ ! -d distrib ]; then
	mkdir distrib
fi
if [ -d distrib/LDView ]; then
	rm -rf distrib/LDView
fi
mkdir distrib/LDView
FileVersion=`plutil -p "${app_root}/Contents/Info.plist" | grep CFBundleShortVersionString | cut -d= -f2 | cut -d\" -f2 | sed "s/ /_/g"`
#FileVersion=`grep 'VALUE "FileVersion"' ../../LDView.rc | cut -d, -f2 | cut -d\" -f2 | cut -d\\\\ -f1 | sed "s/ /_/g"`
Filename="LDView_${FileVersion}"
echo Copying files...
cp ../../8464.mpd distrib/LDView/
cp ../../ChangeHistory.html distrib/LDView
cp -R "${app_root}" distrib/LDView/
cp ../../license.txt distrib/LDView
cp ../../m6459.ldr distrib/LDView
echo Building DMG...
hdiutil create -ov -fs 'HFS+' -srcfolder distrib/LDView -volname "$Filename" -format UDBZ "distrib/$Filename.dmg"
open distrib/
