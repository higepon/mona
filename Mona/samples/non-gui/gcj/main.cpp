#include <monapi.h>
#include <java/lang/System.h>
#include <java/io/PrintStream.h>
#include "A.h"
//#include <stddef.h>

//int main() {
int MonaMain(List<char*>* pekoe) {
	::java::lang::System::out = new ::java::io::PrintStream(NULL);
	::A::main();
	return 0;
}
