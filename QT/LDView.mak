DIRECTORIES = ../TCFoundation ../LDLoader ../TRE ../LDLib
QMAKE = qmake

all: Makefile
	for DIR in $(DIRECTORIES); do	\
		cd $$DIR; $(MAKE); cd ../QT;	\
	done;
	make

debug: Makefile
	for DIR in $(DIRECTORIES); do	\
		cd $$DIR; $(MAKE) debug; cd ../QT;	\
	done;
	make

clean:
	for DIR in $(DIRECTORIES); do		\
		cd $$DIR; $(MAKE) clean; cd ../QT;	\
	done;
	make clean

dotDirs: .ui .moc .obj .test
	if [ "$?" != "" ]; then			\
		for DIR in $?; do			\
			if [ ! -d $$DIR ]; then	\
				mkdir $$DIR;		\
			fi;						\
		done;						\
	fi;

.ui:
.moc:
.obj:
.test:

Makefile: LDView.pro $(QTDIR)/mkspecs/default/qmake.conf
	$(QMAKE) -o Makefile LDView.pro
