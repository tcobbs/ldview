#!/bin/sh
LIST=Dockerfile-*
if [ -n "$1" ] ; then LIST=$1 ; fi
for f in $LIST ; do
	name=`echo $f|sed 's/Dockerfile-//'`
	tag=`echo $name|sed 's/_/:/'`
	docker build --network host -t ldview-$tag -f Dockerfile-$name .
done
