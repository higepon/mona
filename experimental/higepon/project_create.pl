#!/usr/bin/perl -w
use strict;
use IO::File;
use DateTime;
use Readonly;

Readonly my $AUTHOR => 'Higepon';
my $main_cpp = <<'EOF';
/*!
  \file   main.cpp
  \brief  %s

  Copyright (c) %s %s
  WITHOUT ANY WARRANTY

  \author  %s
  \version $Revision$
  \date   create:%s update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

int main(int argc, char *argv[])
{
    return 0;
}
EOF

# this is tekito script.
sub main {
    my $name = shift @ARGV or die;
    my $dt = DateTime->now(time_zone => 'Asia/Tokyo');
    my $today = $dt->ymd('/');
    my $year  = $dt->year;
    my $dir    = "$name";
    my $srcdir = "$dir/src";
    system("mkdir -p $srcdir");
    make_file("$dir/Makefile.am"   , "SUBDIRS = src\n");
    system("touch $dir/configure.ac");
    make_file("$srcdir/Makefile.am", sprintf("bin_PROGRAMS = %s\n%s_SOURCES = main.cpp\n", $name, $name));
    make_file("$srcdir/main.cpp"   , sprintf($main_cpp, $name, $year, $AUTHOR, $AUTHOR, $today));
    system("cd $dir && autoscan");
    system("cd $dir && mv configure.scan configure.tmp");
    system("sed 's/FULL-PACKAGE-NAME, VERSION/$name, 0.0.1/g' $dir/configure.tmp | sed '6 i AM_INIT_AUTOMAKE([foreign])' > $dir/configure.ac && rm $dir/configure.tmp");
    system("cd $dir && touch NEWS README AUTHORS ChangeLog");
    system("cd $dir && autoheader && /usr/local/bin/aclocal && /usr/local/bin/automake -a -c && autoconf");
    system("cd $dir && /usr/local/bin/aclocal");
    system("cd $dir && autoheader && /usr/local/bin/automake -a -c");
    system("cd $dir && autoconf");
    system("rm -rf $dir/autom4te.cache");
    system("cd $dir && ./configure && make");
}

sub make_file {
    my $file = shift or return;
    my $content = shift or return;
    if (my $f = IO::File->new("> $file")) {
        print $f $content;
        print qq/created "$file"\n/;
    }
}

main();
