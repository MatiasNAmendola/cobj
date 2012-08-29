#!/bin/bash

for f in $(find . -name "*.co"); do
    echo $f
    time (../co $f &>/dev/null)
done
