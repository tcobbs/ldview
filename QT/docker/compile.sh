#!/bin/sh
LIST=Dockerfile-*
if [ -n "$1" ] ; then LIST=$1 ; fi
for f in $LIST ; do
	name=`echo $f|sed 's/Dockerfile-//'`
	namen=`echo $name|tr -d ':.'`
	docker run -v $PWD:/mnt/lego --rm=true --name ldview-$namen ldview-$name
done
