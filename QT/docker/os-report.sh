#!/bin/sh
echo OS,Qt5,gcc,gl2ps,tinyxml,zlib,libpng,libjpeg,OSMesa,hostname,which
for os in archlinux oraclelinux:8 fedora:latest mageia:8 opensuse/leap debian:11 ubuntu:22.04 alpine:latest; do
	linux=`echo $os| cut -f1 -d:`
	linux=`echo $linux|cut -f1 -d/`
	echo -n $os,
	docker run --name $linux-report --rm=true --network host -v $PWD:/opt/ -it $os /opt/query-package.sh 2>/dev/null
done
