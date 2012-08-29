#!/bin/bash
# 
# indent should be gun indent
#
INDENT=indent
OUTSIDE_TYPEDEFS+=FILE

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
        if (a && match($0, "^} [a-zA-Z0-9_]+;")) {
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
for f in $(find . -name "*.[ch]"  |  grep -E -v '^\.\/(argparse|linenoise|parser|scanner)'); do
    lines=$(($lines + $(wc $f -l | cut -d ' ' -f 1)))
    $INDENT $f
done
printf "%d lines code indented.\n" $lines

# clear
rm -f .indent.pro
find . -name "*~" | xargs rm -f
