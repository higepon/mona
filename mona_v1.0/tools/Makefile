all: fstest.cpp HogeDriver.cpp HogeDriver.h ../src/FAT12.cpp ../src/include/FAT12.h
	g++ -o test.exe fstest.cpp HogeDriver.cpp ../src/FAT12.cpp ../src/string.cpp ../src/BitMap.cpp -idirafter ../src/include
	etags ./*.cpp ./*.h ../src/*.cpp ../src/include/*.h
