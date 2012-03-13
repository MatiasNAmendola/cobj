#!/bin/bash
OUTSIDE_TYPEDEFS+=FILE

test -e ~/.indent.pro && cp ~/.indent.pro .indent.pro
# try to find out all typenames defined by 'typedef' of c
# case 1: typedef in oneline
sed -n 's/\s*typedef\s.*\s\([a-zA-Z_]\+\);/\1/p' *.[ch] \
    | xargs -Itype echo -T type >> .indent.pro

# case 2: typedef span multiline
find . -name "*.[ch]" | xargs \
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
        }
    }
}

END {
    for (i in typedefs) {
        print typedefs[i]
    }
}' \
    | xargs -Itype echo -T type >> .indent.pro

# typedefs from outside libraries
for t in $OUTSIDE_TYPEDEFS; do
    echo "-T $t" >> .indent.pro
done

indent *.[ch]
rm -f .indent.pro
rm -f *~
