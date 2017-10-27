FROM ubuntu:17.10
MAINTAINER Peter Bartfai pbartfai@stardust.hu

RUN apt-get update ; apt-get install -y git lintian build-essential debhelper ccache lsb-release
RUN git clone https://github.com/tcobbs/ldview
#RUN cd ldview/QT;mk-build-deps -t "apt-get -y" -r -i debian/control
RUN apt-get install -y `grep Build-Depends ldview/QT/debian/control | cut -d: -f2| sed 's/(.*)//g' | tr -d ,` libtinyxml-dev libgl2ps-dev

VOLUME /mnt/lego
CMD cd ldview/QT ; \
	git pull; \
	./makedeb -qt5 ; \
	if [ -d /mnt/lego ] ; then \
		cp -f ldview*.deb /mnt/lego/ ; \
	fi
