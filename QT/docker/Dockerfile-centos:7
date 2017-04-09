FROM centos:7
MAINTAINER Peter Bartfai pbartfai@stardust.hu

RUN yum install -y git rpm-build rpmlint ccache
RUN git clone https://github.com/tcobbs/ldview
RUN yum-builddep -y ldview/QT/LDView.spec
RUN cd ldview/QT ; \
	cp -f LDView.spec LDView-qt5.spec ;\
	sed 's/define qt5 0/define qt5 1/' -i LDView-qt5.spec ;\
	yum-builddep -y LDView-qt5.spec

VOLUME /mnt/lego
CMD yum update -x kernel* -y; \
	cd ldview/QT ; \
	git pull; \
	rpmbuild -bb LDView.spec ; \
	cp -f LDView.spec LDView-qt5.spec ;\
	sed 's/define qt5 0/define qt5 1/' -i LDView-qt5.spec ;\
	rpmbuild -bb LDView-qt5.spec ;\
#	export LDVDEV32=YES ;\
#	rpmbuild --target=i686 -bb LDView.spec ; \
	for r in /root/rpm*/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm ; do \
		if [ -f $r ] ; then \
			if [ -d /mnt/lego ] ; then \
				cp -f $r /mnt/lego ; \
			fi ; \
			rpmlint $r ; \
		fi; \
	done
