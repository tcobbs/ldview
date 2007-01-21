#!/bin/sh
#
# usage inidiff.sh <language>
#
#

keys() {
	key=$1
	grep = $key |\
	grep -v '^;' |\
	awk -F= ' {print $1}' |\
	sed 's/[0-9]* *$//g' |\
	sort | uniq
}
keys ../LDViewMessages.ini >/tmp/inidiff.$$.main
keys ../Translations/$1/LDViewMessages.ini >/tmp/inidiff.$$.$1
#grep -v "`keys ../LDViewMessages.ini`"
diff /tmp/inidiff.$$.main /tmp/inidiff.$$.$1 | grep '<' | sed 's/< //g'
rm -f /tmp/inidiff.$$.main /tmp/inidiff.$$.$1
