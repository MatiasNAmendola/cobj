#!/bin/bash
# 
# Bench *.co codes and record results.
#
# Author: Yecheng Fu <cofyc.jackson@gmail.com> 

report=results/$(git describe --match "v[0-9]*" HEAD).report

# clear first
echo -n > $report

# system summary information
echo "------ system summary at $(date) ------" >> $report
uname -a >> $report

# bench
echo "------ benchmarks -----" >> $report
for f in $(find . -name "*.co"); do
    printf ">>> %s\n" $f >> $report
    cat $f >> $report
    printf "<<< time\n" >> $report
    time (../co $f &>/dev/null) 2>> $report
    echo >> $report
done
