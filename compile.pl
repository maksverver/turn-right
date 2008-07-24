#!/usr/bin/perl
use strict;

my %incl = {};

sub include
{
    my ($fn) = @_;
    return if $incl{$fn};
    $incl{$fn} = 1;

    local(*FP);
    open FP, $fn or die $!;
    while(my $line=<FP>)
    {
        if($line =~ /#include "(.+?)"/) {
            print "/* BEGIN of HEADER file $fn */\n";
            include($1);
            print "/* END of HEADER file $fn */\n";
        } elsif($line =~ /#include <(.+?)>/) {
            if(!$incl{$1}) {
                $incl{$1} = 1;
                print $line;
            }
        } else {
            print $line;
        }

    }
    close FP;
}


foreach my $fn (@ARGV) {
    print "/* BEGIN of SOURCE file $fn */\n";
    include $fn;
    print "/* END of SOURCE file $fn */\n";
}
