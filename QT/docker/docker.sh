#!/bin/sh
#LDVIEW_BRANCH=master
LIST=Dockerfile-*
if [ -n "$1" ] ; then LIST=$1 ; fi
for f in $LIST ; do
	name=`echo $f|sed 's/.*Dockerfile-//'`
	tag=`echo $name|sed 's/_/:/'`
	docker build --network host -t ldview-$tag $(test -n "$LDVIEW_BRANCH" && echo --build-arg LDVIEW_BRANCH=$LDVIEW_BRANCH) -f $f .
done
