use v5;
use strict;

sub fibo {
    my $n = shift;
    if ($n == 0) {
        return 0;
    }
    if ($n == 1) {
        return 1;
    }
    return fibo($n - 1) + fibo($n - 2);
}

my $i = 0;
for ($i = 0; $i < 30; $i++) {
    print(fibo($i), "\n");
}
