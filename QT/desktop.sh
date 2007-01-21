#!/bin/sh
cp -f desktop/ldraw.mime          /usr/share/mime-info/
cp -f desktop/ldraw.xml           /usr/share/mime/packages/
cp -f desktop/ldraw.keys          /usr/share/mime-info/
cp -f desktop/ldview.applications /usr/share/application-registry/
cp -f desktop/ldraw.desktop      /usr/share/applications/
cp -f desktop/ldraw-thumbnailer   /usr/bin/

update-mime-database  /usr/share/mime >/dev/null
update-desktop-database

cp -f images/LDViewIcon.png /usr/share/pixmaps/gnome-ldraw.png
cp -f images/LDViewIcon.png \
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
cp -f images/LDViewIcon.png \
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png


chmod +x /usr/bin/ldraw-thumbnailer
cp -f desktop/ldraw.schemas /etc/gconf/schemas/

cd /etc/gconf/schemas
GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
gconftool-2 --makefile-install-rule ldraw.schemas >/dev/null

kill -HUP `pidof nautilus`

