all : ./hashtest.exe

./hashtest.exe : ./hashtest.cpp ../../src/include/collection.h
	g++ -g hashtest.cpp -o hashtest.exe -idirafter ../../src/include -idirafter ../../src/user
