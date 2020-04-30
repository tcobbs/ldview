#!/bin/sh
LIST=Dockerfile-*
if [ -n "$1" ] ; then LIST=$1 ; fi
for f in $LIST ; do
	name=`echo $f|sed 's/Dockerfile-//'`
	namen=`echo $name|tr -d ':.'`
	tag=`echo $name|sed 's/_/:/'`
	docker run -v $PWD:/mnt/lego --rm=true --network host --name ldview-$namen ldview-$tag
done
