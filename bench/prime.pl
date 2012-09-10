use v5;
use strict;

my $maxnum = 100000;

sub is_prime {
    my $n = shift;
    my $j = 2;
    while ($j * $j < $n + 1) {
        if ($n % $j == 0) {
            return 0;
        }
        $j++;
    }
    return 1;
}

for (my $i = 2; $i <= $maxnum; $i++) {
    if (is_prime($i)) {
        print($i, "\n");
    }
}
