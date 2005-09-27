#include <gcj/javaprims.h>
#include <gcj/cni.h>
#include <sms_gc.h>
#include "A.h"
//#include "JNoiz2bg.h"

#ifdef MONA
#include <monapi.h>
int MonaMain(List<char*>* pekoe) {
#else
int main() {
#endif
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	::A::main(NULL);
	//::JNoiz2bg::main(NULL);
	return 0;
}
