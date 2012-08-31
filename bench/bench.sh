#!/bin/bash
#
# TODO
#

ROOTPATH=$(dirname $0)

cd $ROOTPATH
for f in $(find . -type f); do
    if [[ $f == './bench.sh' ]]; then
        continue
    fi
    if [[ $f =~ \.co$ ]]; then
        echo $f
        time $ROOTPATH/../cobj $f > /dev/null
    fi
done
