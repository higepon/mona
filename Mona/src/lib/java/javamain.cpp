#include <sms_gc.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#else
#include <locale.h>
#endif

class JAVAMAIN : public ::java::lang::Object
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
		elements(args)[i - 1] = JvNewStringUTF(pekoe->get(i));
	JAVAMAIN::main(args);
	return 0;
}
#else
int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "");
	SMS_GC_INIT();
	JvCreateJavaVM(NULL);
	jstringArray args = (jstringArray)JvNewObjectArray(argc - 1, &::java::lang::String::class$, NULL);
	for (int i = 1; i < argc; i++)
		elements(args)[i - 1] = JvNewStringUTF(argv[i]);
	JAVAMAIN::main(args);
	return 0;
}
#endif
