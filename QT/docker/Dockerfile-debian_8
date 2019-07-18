FROM debian:8 
MAINTAINER Peter Bartfai pbartfai@stardust.hu

ADD install-devel-packages.sh /
RUN ./install-devel-packages.sh

VOLUME /mnt/lego
CMD cd ldview/QT ; \
	git pull; \
	./makedeb -qt5 ; \
