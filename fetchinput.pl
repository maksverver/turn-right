#!/usr/bin/perl

if($#ARGV != 1) {
    print "Usage: $0 <game id> <player>\n" ;
    exit;
}

if(!($ARGV[0] > 0)) {
    die "Invalid game id";
}

# Read file
open(PIPE, "curl --stderr /dev/null 'http://www.codecup.nl/showgame.php?ga=$ARGV[0]' |");
while(<PIPE>) {
    $white = $1 if(/<param name="white" value="(.+)">/);
    $black = $1 if(/<param name="black" value="(.+)">/);
    @moves = split /,/, $1 if(/<param name="moves" value="(.+)">/);
}
close(PIPE);

splice @moves, 35, 1;

# Output requested data
if($ARGV[1] eq 'white' || $ARGV[1] eq $white)
{
    print "Start\n";
    for($n = 1; $n <= $#moves; $n += 2) {
        print "$moves[$n]\n"
    }
}
elsif($ARGV[1] eq 'black' || $ARGV[1] eq $black)
{
    for($n = 0; $n <= $#moves; $n += 2) {
        print "$moves[$n]\n"
    }
}
else
{
    die("No player named '$ARGV[1]' found!");
}
