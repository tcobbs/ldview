#!/bin/sh
echo OS,Qt5,gcc,gl2ps,tinyxml,zlib,libpng,libjpeg,OSMesa
for os in archlinux/base centos:8 fedora:33 mageia:7 opensuse/leap debian:10 ubuntu:20.04 alpine:latest; do
	linux=`echo $os| cut -f1 -d:`
	linux=`echo $linux|cut -f1 -d/`
	echo -n $os,
	docker run --name $linux-report --rm=true --network host -v $PWD:/opt/ -it $os /opt/query-package.sh 2>/dev/null
done
