#!/usr/bin/perl

while(<>) {
    chomp;
    @parts = split /-/;

    $smallest = 'x9';
    for($n = 0; $n <= $#parts; ++$n)
    {
        $elem = pop @parts;
        unshift @parts, $elem;
        if($elem lt $smallest) {
            $rep      = join('-', @parts) ;
            $smallest = $elem;
        }
    }
    print $rep, "\n";
}
