#!/bin/bash
# 
# Bench *.co codes and record results.
#
# Author: Yecheng Fu <cofyc.jackson@gmail.com> 



# bench
for f in $(find . -name "*.co"); do
    report=results/$f.report
    # header
    echo "------ bench $(git describe --match 'v[0-9]*' HEAD) at $(date) ------" >> $report
    uname -a >> $report
    echo >> $report
    printf ">>> %s\n" $f >> $report
    cat $f >> $report
    printf "<<< time\n" >> $report
    time (../co $f &>/dev/null) 2>> $report
    echo >> $report
done
