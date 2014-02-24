#!/bin/sh
cp -f desktop/ldraw.mime          /usr/share/mime-info/
cp -f desktop/ldraw.xml           /usr/share/mime/packages/
cp -f desktop/ldraw.keys          /usr/share/mime-info/
cp -f desktop/ldview.applications /usr/share/application-registry/
cp -f desktop/ldview.desktop      /usr/share/applications/
cp -f desktop/ldraw-thumbnailer   /usr/bin/

update-mime-database  /usr/share/mime >/dev/null
update-desktop-database

cp -f images/LDViewIcon.png /usr/share/pixmaps/gnome-ldraw.png
cp -f images/LDViewIcon.png \
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
cp -f images/LDViewIcon.png \
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png


chmod +x /usr/bin/ldraw-thumbnailer
GCONF_SCHEMA_DIR=""
if [ -d /etc/gconf/schemas ]; then
	GCONF_SCHEMA_DIR=/etc/gconf/schemas
elif [ -d /usr/share/gconf/schemas ]; then
	GCONF_SCHEMA_DIR=/usr/share/gconf/schemas
fi
if [ "$GCONF_SCHEMA_DIR" != "" ]; then
	cp -f desktop/ldraw.schemas ${GCONF_SCHEMA_DIR}/

	cd $GCONF_SCHEMA_DIR
	GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
		gconftool-2 --makefile-install-rule ldraw.schemas >/dev/null
else
	echo "Could not locate gconf schema directory; thumbnails won't work."
fi

PIDOF=""
KilledNautilus=0
# Try to find the pidof command in path.  Note that if the path has directories
# in it that contain spaces, the following won't work right for those
# directories, but it seems unlikely that pidof will be found in one of those
# directories.
for dir in `echo $PATH | tr : ' '`
do
	if [ -x ${dir}/pidof ]; then
		PIDOF=${dir}/pidof
	fi
done
if [ "$PIDOF" = "" ]; then
	# Couldn't find pidof in path; check some standard locations.
	if [ -x /sbin/pidof ]; then
		PIDOF=/sbin/pidof
	elif [ -x /bin/pidof ]; then
		PIDOF=/bin/pidof
	fi
fi
if [ "$PIDOF" != "" ]; then
	NautilusPid=`$PIDOF nautilus`
	if [ "$NautilusPid" != "" ]; then
		kill -HUP $NautilusPid
		KilledNautilus=1
	else
		echo Could not find nautilus task.
	fi
fi
if [ $KilledNautilus -eq 0 ]; then
	echo Please log out and log back in for thumbnail generation to activate.
else
	echo You may have to log out and back in before thumbnail generation starts.
fi

if [ ! -x /usr/bin/LDView ]; then
	make install
fi

