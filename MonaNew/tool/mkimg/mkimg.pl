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

writedir("");

print "output: $g_out\n";
print "Done!\n";

sub call
{
	(my $arg) = @_;
	print $arg, "\n";
	die if system $arg;
}

sub writedir
{
	(my $dir) = @_;
	$dir .= "/" if $dir ne "";
	opendir(DIR, "$g_dir/$dir");
	my @files = readdir(DIR);
	closedir(DIR);
	foreach my $f(@files)
	{
		next if !-f "$g_dir/$dir$f" || $f eq $g_mbr;
		call("fat_write $g_out $g_dir/$dir$f $dir$f");
	}
	foreach my $d(@files)
	{
		next if !-d "$g_dir/$dir$d" || $d eq "." || $d eq "..";
		call("fat_write $g_out --mkdir $dir$d");
		if (index("$dir$d", "APPS", 0) < 0) {
			writedir("$dir$d");
		}
	}
}
