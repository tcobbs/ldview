#! /bin/sh
# Run this script from the root of the Boost source (i.e. boost_1_35_0/).
# It will build Boost as static libraries, and create Universal binaries if run
# on Mac OS X.
if [ "`uname`" = "Darwin" ]; then
	BJAM_EXTRA_CONFIG="architecture=combined link=static stage"
else
	BJAM_EXTRA_CONFIG="link=static stage"
fi
./configure --with-libraries=thread,date_time
cat Makefile | sed "s/BJAM_CONFIG=\$/BJAM_CONFIG=${BJAM_EXTRA_CONFIG}/" > Makefile.new
mv Makefile.new Makefile
make
