all : ./ipc.exe

./ipc.exe : ./ipc.cpp
	g++ -Wall -DIPC_TEST ipc.cpp -o ipc.exe -idirafter ../../src/include -idirafter ../../src/user
