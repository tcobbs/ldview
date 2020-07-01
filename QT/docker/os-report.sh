#!/bin/sh

for os in archlinux/base centos:8 fedora:32 mageia:7 opensuse/leap debian:10 ubuntu:20.04 ; do
	linux=`echo $os| cut -f1 -d:`
	linux=`echo $linux|cut -f1 -d/`
	echo $os - $linux
	docker run --name $linux-report --rm=true --network host -v $PWD:/opt/ -it $os /opt/query-package.sh
done
