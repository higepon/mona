#include <sms_gc/sms_gc.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#endif

class JAVAMAIN
{
public:
	static void main(jstringArray args);
};

#ifdef MONA
int MonaMain(List<char*>* pekoe) {
	SMS_GC_INIT();
	JvCreateJavaVM((void*)JAVAMAIN::main);
	int argc = pekoe->size();
	jstringArray args = (jstringArray)JvNewObjectArray(argc, &::java::lang::String::class$, NULL);
	for (int i = 0; i < argc; i++)
		elements(args)[i] = JvNewStringUTF(pekoe->get(i));
	JAVAMAIN::main(args);
	return 0;
}
#else
int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "");
	SMS_GC_INIT();
	JvCreateJavaVM((void*)JAVAMAIN::main);
	jstringArray args = (jstringArray)JvNewObjectArray(argc - 1, &::java::lang::String::class$, NULL);
	for (int i = 1; i < argc; i++) {
		int mlen = strlen(argv[i]);
		jcharArray wcs = JvNewCharArray(mlen);
		int wlen = mbstowcs((wchar_t*)elements(wcs), argv[i], mlen);
		elements(args)[i - 1] = new ::java::lang::String(wcs, 0, wlen);
	}
	JAVAMAIN::main(args);
	return 0;
}
#endif
