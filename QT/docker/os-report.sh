#!/bin/sh
echo OS,Qt5,gcc,gl2ps,tinyxml,zlib,libpng,libjpeg,OSMesa,hostname,which,minizip
for os in archlinux oraclelinux:10 fedora:latest mageia:latest opensuse/leap debian:latest ubuntu:latest alpine:latest; do
	linux=`echo $os| cut -f1 -d:`
	linux=`echo $linux|cut -f1 -d/`
	echo -n $os,
	docker run --name $linux-report --rm=true --network host -v $PWD:/opt/ -it $os /opt/query-package.sh 2>/dev/null
done
