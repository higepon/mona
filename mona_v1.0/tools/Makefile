all: ../bin/fat_write.exe

../bin/fat_write.exe : fat_write.cpp ../src/FAT12.cpp ../src/include/FAT12.h ../src/string.cpp ../src/BitMap.cpp
	g++ -DFS_TEST -o ../bin/fat_write.exe fat_write.cpp HogeDriver.cpp ../src/FAT12.cpp ../src/string.cpp ../src/BitMap.cpp -idirafter ../src/include/ -idirafter ../src/user/ -idirafter .

#  ./test.exe: fstest.cpp HogeDriver.cpp HogeDriver.h ../src/FAT12.cpp ../src/include/FAT12.h
#  	g++ -DFS_TEST -o test.exe fstest.cpp HogeDriver.cpp ../src/FAT12.cpp ../src/string.cpp ../src/BitMap.cpp -idirafter ../src/include -idirafter ../src/user
#  	etags ./*.cpp ./*.h ../src/*.cpp ../src/include/*.h

#./elfview.exe: elfTest.cpp ../src/include/elf.h
#	g++ -DFS_TEST -o elfview.exe elfTest.cpp ../src/elf.cpp -idirafter ../src/include -idirafter../src/user

# ./sample.elf: ../src/user/hello.cpp
# 	gcc -nostdlib -fno-exceptions  -fno-rtti -ffreestanding -Wall  -I . -I ../src/user  -c ../src/user/hello.cpp -o ./hello.o
# 	ld -n -Ttext 0xA00000 -static ./hello.o ../src/userlib.o -o sample.elf
# 	objcopy --output-target=elf32-i386 sample.elf
# clean:
# 	rm -f test.exe elfview.exe hello.o sample.elf
