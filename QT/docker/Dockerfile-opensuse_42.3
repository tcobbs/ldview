FROM opensuse:42.3
MAINTAINER Peter Bartfai pbartfai@stardust.hu

RUN zypper --non-interactive install git make libqt4-devel gcc gcc-c++ rpm-build libkde4-devel tinyxml-devel glu-devel rpmlint gl2ps-devel fdupes Mesa-devel
RUN git clone https://github.com/tcobbs/ldview
VOLUME /mnt/lego
CMD cd ldview/QT ; \
	git pull; \
	rpmbuild -bb LDView.spec ; \
	for r in /root/rpm*/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm ; do \
		if [ -f $r ] ; then \
			if [ -d /mnt/lego ] ; then \
				cp -f $r /mnt/lego ; \
			fi ; \
			rpmlint $r ; \
		fi; \
	done
