FROM fedora:25
MAINTAINER Peter Bartfai pbartfai@stardust.hu

# KDE plugin is not properly built: 64 but instead of 32

RUN dnf install -y git rpmlint ccache dnf-plugins-core rpm-build
RUN git clone https://github.com/tcobbs/ldview
RUN dnf -y install qt-devel.i686 zlib-devel.i686 libpng-devel.i686 \
	mesa-libOSMesa-devel.i686 glibc-devel.i686 \
	libjpeg-turbo-devel.i686 gl2ps-devel.i686 tinyxml-devel.i686 \
	mesa-libGLU-devel.i686 libglvnd-devel.i686 \
	cmake.i686 gcc-c++.i686 kdelibs-devel.i686 kdebase-devel libstdc++-devel.i686 glibc-headers.i686 gcc.i686

VOLUME /mnt/lego
CMD dnf update -x kernel* -y; cd ldview/QT ; \
	git pull; \
	export LDVDEV32=YES ;\
	rpmbuild --target=i686 -bb LDView.spec ; \
	for r in /root/rpm*/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm ; do \
		if [ -f $r ] ; then \
			if [ -d /mnt/lego ] ; then \
				cp -f $r /mnt/lego ; \
			fi ; \
			rpmlint $r ; \
		fi; \
	done
