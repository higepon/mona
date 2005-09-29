#include <gcj/javaprims.h>
#include <gcj/cni.h>
#include <sms_gc/sms_gc.h>
#include "Applet1.h"

#ifdef MONA
#include <monapi.h>
int MonaMain(List<char*>* pekoe) {
#else
int main() {
#endif
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	::Applet1::main(NULL);
	return 0;
}
