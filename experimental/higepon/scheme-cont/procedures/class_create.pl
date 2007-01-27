#!/usr/bin/perl -w
use strict;
use IO::File;

sub main {
    my $class_name = shift or die;
    my $namespace = 'monash';

    my $cpp = $class_name . ".cpp";
    my $h   = $class_name . ".h";

    -e $cpp and die "$cpp exists!";
    -e $h   and die "$h exists!";
    make_header($h, $class_name, $namespace);
    make_cpp($h, $cpp, $class_name, $namespace);

}
sub get_include_guard_str {
    my $str = shift;
    $str =~ s/\.h//g;
    return sprintf("_PRIMITIVE_PROCEDURE_%s_", uc($str));
}

sub make_file {
    my $file = shift or return;
    my $content = shift or return;
    if (my $f = IO::File->new("> $file")) {
        print $f $content;
        print qq/..\/src\/PrimitiveProcedures\/$file\\\n/;
    }
}

sub make_cpp {
    my $h = shift or return;
    my $cpp = shift or return;
    my $class_name = shift or return;
    my $d_class_name = lc($class_name);
    my $namespace = shift;
    my $cpp_content  = "#include \"$h\"\n\n";
    $cpp_content    .= "using namespace $namespace;\n\n" if $namespace;
    my $content = <<"EOF";
using namespace std;

${class_name}::${class_name}()
{
}

${class_name}::~${class_name}()
{
}

std::string ${class_name}::toString()
{
    return "procedure:${d_class_name}";
}

Object* ${class_name}::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* ${class_name}::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 1)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "${d_class_name} got %d arguments, but required %d", as->size(), 1);
        return NULL;
    }
    return NULL;
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
    $header_content    .= '#include "PrimitiveProcedure.h"' . "\n\n";
    $header_content    .= "namespace $namespace {\n\n" if $namespace;
    my $content = <<"EOF";
class $class_name : public PrimitiveProcedure
{
public:
    $class_name();
    virtual ~$class_name();

    virtual std::string toString();
    virtual Object* eval(Environment* env);
    virtual Object* apply(Objects* arguments, Environment* env);
};

EOF
    $header_content .= $content;
    $header_content .= "}; // namespace $namespace\n\n" if $namespace;
    $header_content .= "#endif // $include_guard\n";
    &make_file($h, $header_content);
}

main(shift, shift);
