FROM mageia:6
MAINTAINER Peter Bartfai pbartfai@stardust.hu

RUN dnf install -y git rpm-build rpmlint dnf-plugins-core
RUN git clone https://github.com/tcobbs/ldview
RUN dnf builddep -y ldview/QT/LDView.spec
RUN cd ldview/QT ; \
	cp -f LDView.spec LDView-qt5.spec ;\
	sed 's/define qt5 0/define qt5 1/' -i LDView-qt5.spec ;\
         dnf builddep -y LDView-qt5.spec

VOLUME /mnt/lego
ENV PATH /usr/lib64/qt4/bin:$PATH
CMD cd ldview/QT ; \
	git pull; \
	rpmbuild -bb LDView.spec ; \
	cp -f LDView.spec LDView-qt5.spec ;\
	sed 's/define qt5 0/define qt5 1/' -i LDView-qt5.spec ;\
	PATH=/usr/lib64/qt5/bin:$PATH ;\
	export PATH ;\
	rpmbuild -bb LDView-qt5.spec ;\
	for r in /root/rpm*/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm ; do \
		if [ -f $r ] ; then \
			if [ -d /mnt/lego ] ; then \
				cp -f $r /mnt/lego ; \
			fi ; \
			rpmlint $r ; \
		fi; \
	done
