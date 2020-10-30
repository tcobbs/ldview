FROM ubuntu:20.04
MAINTAINER Peter Bartfai pbartfai@stardust.hu

ENV DEBIAN_FRONTEND=noninteractive
ADD install-devel-packages.sh /
RUN ./install-devel-packages.sh

VOLUME /mnt/lego
CMD cd ldview/QT ; \
	git pull; \
	sed -i 's/kdelibs5-dev//g' debian/control ; \
	./makedeb -qt5 ; \
