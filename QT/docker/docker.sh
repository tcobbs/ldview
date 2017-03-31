#!/bin/sh
LIST=Dockerfile-*
if [ -n "$1" ] ; then LIST=$1 ; fi
for f in $LIST ; do
	name=`echo $f|sed 's/Dockerfile-//'`
	docker build -t ldview-$name -f Dockerfile-$name .
done
