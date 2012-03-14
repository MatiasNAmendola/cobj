#!/bin/bash
OUTSIDE_TYPEDEFS+=FILE

test -e ~/.indent.pro && cp ~/.indent.pro .indent.pro

## try to find out all typenames defined by 'typedef' of c
function print_typedef() {
    # case 1: typedefs in oneline
    sed -n 's/\s*typedef\s.*\s\([a-zA-Z_]\+\);/\1/p' $1

    # case 2: typedefs (function) in oneline
    sed -nr 's/\s*typedef\s*.*\(\*([a-zA-Z_]+)\)\s*\(.*;/\1/p' $1

    # case 3: typedefs span multiline
    awk '
    BEGIN {
        a = 0
    }

    {
        if (match($0, "typedef")) {
            a = 1
        }
        if (a && match($0, "} [a-zA-Z0-9_]+;")) {
            if (x = index($2, ";")) {
                typedefs[$0] = substr($2, 0, x - 1);
                a = 0
            }
        }
    }

    END {
        for (i in typedefs) {
            print typedefs[i]
        }
    }' $1
}

for f in $(find . -name "*.[ch]"); do
    print_typedef $f | xargs -Itype echo -T type >> .indent.pro
done

# typedefs from outside libraries
for t in $OUTSIDE_TYPEDEFS; do
    echo "-T $t" >> .indent.pro
done

# indent
lines=0
for f in $(find . -name "*.[ch]"  |  grep -E -v '^\.\/(argparse|dstring)'); do
    lines=$(($lines + $(wc co.c -l | cut -d ' ' -f 1)))
    indent $f
done
printf "%d lines code indented.\n" $lines

# clear
rm -f .indent.pro
find . -name "*~" | xargs rm -f
