#!/usr/bin/perl -w
use strict;
use IO::File;

sub main {
    my $class_name = shift or die;
    my $namespace = shift;

    my $cpp = $class_name . ".cpp";
    my $h   = $class_name . ".h";

    -e $cpp and die "$cpp exists!";
    -e $h   and die "$h exists!";
    make_header($h, $class_name, $namespace);
    make_cpp($h, $cpp, $class_name, $namespace);

}
sub get_include_guard_str {
    my $str = shift;
    $str =~ s/\./\_/g;
    return sprintf("__%s__", uc($str));
}

sub make_file {
    my $file = shift or return;
    my $content = shift or return;
    if (my $f = IO::File->new("> $file")) {
        print $f $content;
        print qq/created "$file"\n/;
    }
}

sub make_cpp {
    my $h = shift or return;
    my $cpp = shift or return;
    my $class_name = shift or return;
    my $namespace = shift;
    my $cpp_content  = "#include \"$h\"\n\n";
    $cpp_content    .= "using namespace $namespace;\n\n" if $namespace;
    my $content = <<"EOF";
${class_name}::${class_name}()
{
}

${class_name}::~${class_name}()
{
}
EOF
    $cpp_content .= $content;
    &make_file($cpp, $cpp_content);
}

sub make_header {
    my $h = shift or return;
    my $class_name = shift or return;
    my $namespace = shift;
    my $include_guard = get_include_guard_str($h);
    my $header_content  = "#ifndef $include_guard\n";
    $header_content    .= "#define $include_guard\n";
    $header_content    .= "\n";
    $header_content    .= "namespace $namespace {\n\n" if $namespace;
    my $content = <<"EOF";
class $class_name
{
public:
    $class_name();
    virtual ~$class_name();
};

EOF
    $header_content .= $content;
    $header_content .= "}; // namespace $namespace\n\n" if $namespace;
    $header_content .= "#endif // $include_guard";
    &make_file($h, $header_content);
}

main(shift, shift);
