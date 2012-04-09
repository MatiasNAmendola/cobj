#!/bin/bash
# 
# Author: Yecheng Fu <cofyc.jackson@gmail.com> 

# bench
for f in $(find . -name "*.co"); do
    echo $f
    time (../co $f &>/dev/null)
done
