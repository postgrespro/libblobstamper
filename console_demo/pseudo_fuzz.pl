#!/usr/bin/perl

use strict;
use Path::Tiny;
use bytes;

die "file 'buf' not found" unless -e "buf";

my $size = -s "buf";

my $offset = int(rand($size));
my $value = int(rand(256));

open(my $out, '+<:raw', 'buf');
seek($out, $offset, 0);
print $out pack('C', $value);

close $out;

