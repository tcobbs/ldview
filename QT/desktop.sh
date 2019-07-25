#!/bin/sh
cp -f desktop/ldraw.mime          /usr/share/mime-info/
cp -f desktop/ldraw.xml           /usr/share/mime/packages/
cp -f desktop/ldraw.keys          /usr/share/mime-info/
cp -f desktop/ldview.applications /usr/share/application-registry/
cp -f desktop/ldview.desktop      /usr/share/applications/
cp -f desktop/ldraw-thumbnailer   /usr/bin/
cp -f desktop/ldview.thumbnailer  /usr/share/thumbnailers/

update-mime-database  /usr/share/mime >/dev/null
update-desktop-database

cp -f images/LDViewIcon.png /usr/share/pixmaps/gnome-ldraw.png
cp -f images/LDViewIcon.png \
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
cp -f images/LDViewIcon.png \
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png


chmod +x /usr/bin/ldraw-thumbnailer

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

