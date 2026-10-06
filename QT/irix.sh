#!/bin/sh
rm -rf /tmp/dist /tmp/build
mkdir -p /tmp/build/usr/bin /tmp/build/usr/share/ldview /tmp/dist
cp -f LDView /tmp/build/usr/bin/
cp -f ../Translations/Hungarian/LDViewMessages.ini /tmp/build/usr/share/ldview/LDViewMessages_hu.ini
cp -f ../Translations/German/LDViewMessages.ini /tmp/build/usr/share/ldview/LDViewMessages_de.ini
cp -f ../Translations/Czech/LDViewMessages.ini /tmp/build/usr/share/ldview/LDViewMessages_cs.ini
cp -f ../Translations/Italian/LDViewMessages.ini /tmp/build/usr/share/ldview/LDViewMessages_it.ini
cat ../LDViewMessages.ini ../LDExporter/LDExportMessages.ini >/tmp/build/usr/share/ldview/LDViewMessages.ini
cp -f ldview_*.qm ../8464.mpd ../m6459.ldr /tmp/build/usr/share/ldview/
gendist -v -sbase /tmp/build -idb ldview.idb -spec ldview-irix.spec -dist /tmp/dist -all
cd /tmp/dist
tar cvf ../ldview.tardist *
cd ..
#rm -rf build dist
