all : ./mem.exe

./mem.exe : ./mem.cpp ../../src/MemoryManager.cpp ../../src/include/MemoryManager.h
	g++ -g -D MM_TEST mem.cpp ../../src/MemoryManager.cpp -o mem.exe -idirafter ../../src/include -idirafter ../../src/user
