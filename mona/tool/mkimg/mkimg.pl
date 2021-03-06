$g_out = "mona.img";
$g_mbr = "firstboot.bin";

$g_dir = $ENV{"MONADIR"};
die "Please set MONADIR!" if $g_dir eq "";
print "$g_dir\n";
die "MONADIR/bin does not exist!" if !-d "$g_dir/bin";
mkdir("$g_dir/bin/APPS", 0777) if !-d "$g_dir/bin/APPS";

call("cp $g_dir/share/fat_write/fat_template.img $g_out");

$g_dir .= "/bin";
call("fat_write $g_out --mbr $g_dir/$g_mbr");
call("fat_write $g_out MONA.CFG MONA.CFG");
call("fat_write $g_out MONITOR.CFG MONITOR.CFG");
call("fat_write $g_out AUTOEXEC.MSH AUTOEXEC.MSH");
call("fat_write $g_out --mkdir APPS");
call("fat_write $g_out --mkdir LIBS");
call("fat_write $g_out --mkdir LIBS/SCHEME");
#call("fat_write $g_out --mkdir MUSIC");

writedir("");

print "output: $g_out\n";
print "Done!\n";

sub call {
    (my $arg) = @_;
    print $arg, "\n";
    if (system $arg) {
        my $image = "$g_dir/bin/$g_out";
        unlink("$g_dir/bin/$g_out") if -d $image;
        die;
    }
}

sub writedir {
    (my $dir) = @_;
    $dir .= "/" if $dir ne "";
    opendir(DIR, "$g_dir/$dir");
    my @files = readdir(DIR);
    closedir(DIR);
    foreach my $f (@files) {
        next if !-f "$g_dir/$dir$f" || $f eq $g_mbr;
        call("fat_write $g_out $g_dir/$dir$f $dir$f");
    }
    foreach my $d (@files) {
        next if !-d "$g_dir/$dir$d" || $d eq "." || $d eq "..";
        next if ("$dir$d" eq 'LIBS' || "$dir$d" eq 'APPS' || "$dir$d" eq 'MUSIC' || "$dir$d" =~ /MESA/);
        call("fat_write $g_out --mkdir $dir$d");
        writedir("$dir$d");
    }
}
