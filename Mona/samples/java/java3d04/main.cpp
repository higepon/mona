#include <gcj/javaprims.h>
#include <gcj/cni.h>
#include <sms_gc/sms_gc.h>
#include "Applet1.h"
#include "Render.h"

#ifdef MONA
#include <monapi.h>
int MonaMain(List<char*>* pekoe) {
#else
int main() {
#endif
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	// register static members for GC
	sms_gc_register_range(&Render::mMaster, &Render::maxv);
	::Applet1::main(NULL);
	return 0;
}
