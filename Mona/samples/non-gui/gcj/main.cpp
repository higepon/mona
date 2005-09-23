#include <gcj/javaprims.h>
#include <gcj/cni.h>
#include <sms_gc.h>
#include "A.h"

#ifdef MONA
#include <monapi.h>
int MonaMain(List<char*>* pekoe) {
#else
int main() {
#endif
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	::A::main();
	return 0;
}
