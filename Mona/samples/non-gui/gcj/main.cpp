#include <java/lang/System.h>
#include <java/io/PrintStream.h>
#include "A.h"
#ifdef MONA
#include <monapi.h>
#else
#include <stddef.h>
#endif
#include <sms_gc/sms_gc.h>

#ifdef MONA
int MonaMain(List<char*>* pekoe) {
#else
int main() {
#endif
	SMS_GC_INIT();
	sms_gc_register(&::java::lang::System::out);
	::java::lang::System::out = new ::java::io::PrintStream(NULL);
	::A::main();
	return 0;
}
