FROM debian:7
MAINTAINER Peter Bartfai pbartfai@stardust.hu

RUN apt-get update ; apt-get install -y git lintian build-essential debhelper ccache devscripts
RUN git clone https://github.com/tcobbs/ldview
RUN cd ldview/QT;mk-build-deps -t "apt-get -y" -r -i debian/control

VOLUME /mnt/lego
CMD cd ldview/QT ; \
	git pull; \
	./makedeb ; \
	if [ -d /mnt/lego ] ; then \
		cp -f ldview*.deb /mnt/lego/ ; \
	fi
