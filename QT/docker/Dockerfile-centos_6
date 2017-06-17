FROM centos:6
MAINTAINER Peter Bartfai pbartfai@stardust.hu

RUN yum install -y git rpm-build rpmlint ccache yum-utils
RUN git clone https://github.com/tcobbs/ldview
RUN yum-builddep -y ldview/QT/LDView.spec

VOLUME /mnt/lego
CMD yum update -x kernel* -y; \
	cd ldview/QT ;\
	git pull; \
	rpmbuild -bb LDView.spec ;\
	for r in /root/rpm*/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm ; do \
		if [ -f $r ] ; then \
			if [ -d /mnt/lego ] ; then \
				cp -f $r /mnt/lego ; \
			fi ; \
			rpmlint $r ; \
		fi; \
	done
