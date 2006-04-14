#include "stdlib.h"

void monapi_initialize_memory(int memorySize);
int user_start_c_impl(int(*monaMain)(int argc, char* argv[]));

int user_start()
{
	monapi_initialize_memory(64 * 1024 * 1024);
	setConstructorList(0);
	return user_start_c_impl(main);
}
