#include <sms_gc/sms_gc.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#else
#include <locale.h>
#endif

class Main : public ::java::lang::Object
{
public:
	static void main(jstringArray args);
};

#ifdef MONA
int MonaMain(List<char*>* pekoe) {
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	int argc = pekoe->size();
	jstringArray args = (jstringArray)JvNewObjectArray(argc, &::java::lang::String::class$, NULL);
	for (int i = 0; i < argc; i++)
		elements(args)[i] = JvNewStringUTF(pekoe->get(i));
	Main::main(args);
	return 0;
}
#else
int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "");
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	jstringArray args = (jstringArray)JvNewObjectArray(argc - 1, &::java::lang::String::class$, NULL);
	for (int i = 1; i < argc; i++) {
		int mlen = strlen(argv[i]);
		jcharArray wcs = JvNewCharArray(mlen);
		int wlen = mbstowcs((wchar_t*)elements(wcs), argv[i], mlen);
		elements(args)[i - 1] = new ::java::lang::String(wcs, 0, wlen);
	}
	Main::main(args);
	return 0;
}
#endif
