#!/usr/bin/perl
use strict;
use warnings;

# we can't use CPAN modules.
# It's hard for beginners.

# requirements
# perl subversion

my $BUILD_DIR = 'mona_build_dir';
my $MONADIR   = '.';

sub main {
    mkdir($BUILD_DIR);
    chdir($BUILD_DIR) or die $!;
    system('svn co https://svn.sourceforge.net/svnroot/monaos/trunk/mona mona') and die $!;
    chdir('mona') or die $!;
    $ENV{MONADIR} = $MONADIR;
    system('./configure') or die $!;
    system('make') or die $!;
}

&main;
